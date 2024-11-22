#include "MainWindow.h"

#include <QImage>
#include <QDebug>
#include <QCameraDevice>
#include <QMediaDevices>

/**
 * Constructor for MainWindow.
 * Initializes the main window with a size of 1000x800.
 * Calls the setup functions for the UI, cascade, camera, and timer.
 * @param parent The parent QWidget for this window.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(1000, 800);

    createUI();

    loadCascade(true);

    detectedObjects objects;

    setCameras();

    // Set up a timer to update frames
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrames);
    timer->start(30);  // Update every 30 ms (~33 FPS)
}

/**
 * Destructor for MainWindow.
 * Releases all camera resources when the window is closed.
 */
MainWindow::~MainWindow() {
    // Release all camera resources
    for (auto &camera : cameras) {
        if (camera.isOpened()) {
            camera.release();
        }
    }
}

/**
 * Loads the Haar Cascade classifier for face detection from the provided path.
 * If the load fails, prints an error message to the console.
 */
void MainWindow::loadCascade(bool pedestrian) {
    // Load the Haar Cascade classifier
    if (pedestrian) {
        pedestrianHOG.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
        usingHog = true;
    }
    else {
        if (!faceCascade.load("../../cascades/haarcascade_frontalface_default.xml")) { // Make sure to provide the correct path
            qDebug() << "Error loading face cascade classifier.";
            return;
        }
    }
}

/**
 * Creates the user interface for the application.
 *
 * This function creates the main window, header and title, grid layout for the cameras, and spacers for the layout.
 */
void MainWindow::createUI() {
    // Creating the containers

    // Central container
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *centralLayout = new QHBoxLayout(centralWidget);

    // Sidebar space
    sidebarWidget = new QWidget(this);
    sidebarWidget->setMinimumWidth(150);
    sidebarWidget->setMinimumHeight(400);
    sidebarWidget->setStyleSheet("background-color: #2E86C1; padding: 3px; margin: 5px; border-radius: 3px;");
    sidebarLayout = new QVBoxLayout(sidebarWidget);

    // *Menu title
    QLabel *menuLabel = new QLabel("Menu", this);
    menuLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold; padding: 5px");
    sidebarLayout->addWidget(menuLabel); // Adding the title space to the container

    // Content container
    contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignCenter);
    centralLayout->addLayout(contentLayout);

    // Title creation and styling
    headerWidget = new QWidget(this); // Title container
    headerWidget->setStyleSheet("background-color: #2E86C1; padding: 10px; margin: 5px; border-radius: 3px;");

    headerLayout = new QVBoxLayout(headerWidget); // Title space

    titleLabel = new QLabel("Camera Viewer", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

    headerLayout->addWidget(titleLabel); // Adding the title space to the container
    contentLayout->addWidget(headerWidget); // Adding the title container to the main container

    // Add a spacer to push other content down
    topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);
    contentLayout->addItem(topSpacer);

    // Cameras space
    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setAlignment(Qt::AlignCenter);
    contentLayout->addLayout(gridLayout);

    bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    contentLayout->addItem(bottomSpacer);

    setCentralWidget(centralWidget);
    setMinimumSize(1000, 800);
    setMaximumSize(1200, 1000);
}

/**
 * Initializes the available cameras, adding camera feeds to the grid layout.
 * 
 * This function detects all available video input devices and sets up a
 * grid layout to display each camera's feed and name. Each camera feed
 * is displayed in a QLabel with a minimum size of 320x240 pixels. The
 * function appends the camera's VideoCapture object and QLabel to
 * their respective lists for further processing.
 * 
 * If a camera is not available, it logs a message and stops processing.
 */
void MainWindow::setCameras() {
    const QList<QCameraDevice> detectedCameras = QMediaDevices::videoInputs();

    int minWidth = 320;  // Width in pixels
    int minHeight = 240; // Height in pixels

    int cameraCount = detectedCameras.size();
    int cameraIndex = 0;
    int row = 0, col = 0;

    for (int i = 0; i < cameraCount; i++) {
        cv::VideoCapture cap(cameraIndex);

        if (!cap.isOpened()) {
            qDebug() << "Camera" << cameraIndex << "not available.";
            break;
        }

        // Create a QLabel to siplay the name in the grid
        QLabel *cameraNameLabel = new QLabel(this);
        cameraNameLabels.append(cameraNameLabel);
        gridLayout->addWidget(cameraNameLabel, row, col);

        // Create a QLabel to siplay the name in the sidebar
        QLabel *sidebarCameraNameLabel = new QLabel(this);
        sidebarCameraNameLabel->setText(QString("CAMERA %1").arg(i));
        sidebarCameraNameLabel->setStyleSheet("background-color: transparent; font-weight: bold; font-size: 15px; padding: 5px;");
        sidebarLayout->addWidget(sidebarCameraNameLabel);

        // Create a QLabel to display the camera feed
        QLabel *cameraLabel = new QLabel(this);
        cameraLabel->setMinimumSize(minWidth, minHeight);
        cameraLabel->setAlignment(Qt::AlignCenter);
        cameraLabels.append(cameraLabel);
        gridLayout->addWidget(cameraLabel, row + 1, col);

        // Create a QComboBox for the camera options
        QComboBox *cameraOption = new QComboBox(sidebarWidget);
        cameraOption->addItem("Default detection");
        cameraOption->addItem("Detection disabled");
        cameraOption->addItem("Alert On detection");
        sidebarLayout->addWidget(cameraOption);

        // Store the VideoCapture object
        cameras.append(std::move(cap));

        // Update grid position
        col++;
        if (col >= 3) {
            col = 0;
            row += 2; // Move down two rows for the next camera feed and its FPS
        }

        cameraIndex++;
    }
}


/**
 * Updates the label style of the camera at the given index to display the alert status.
 *
 * @param val The alert level (0 for no alert, 1 for yellow, 2 for red)
 * @param index The index of the camera in the `cameraLabels` list
 */
void MainWindow::displayAlert(int val, int index) {
    switch (val) {
    case 1:
        cameraNameLabels[index]->setStyleSheet("background-color: yellow; font-weight: bold; font-size: 30px; padding: 5px;");
        break;
    case 2:
        cameraNameLabels[index]->setStyleSheet("background-color: red; font-weight: bold; font-size: 30px; padding: 5px;");
        break;
    default:
        cameraNameLabels[index]->setStyleSheet("background-color: transparent; font-weight: bold; font-size: 30px; padding: 5px;");
        break;
    }
}

/**
 * Updates the frames for each camera, performing face detection and alert checks.
 * 
 * This function iterates over all available cameras, reads frames, and processes them for face detection.
 * Detected faces are marked with red rectangles. The function checks for alert conditions based on the
 * detected object positions and updates the camera label styles accordingly. It converts the processed
 * frames from cv::Mat to QImage format and displays them on the respective QLabel widgets.
 * 
 * Additionally, it removes objects from the detected objects container that have not been updated recently.
 */
void MainWindow::updateFrames() {
    for (int i = 0; i < cameras.size(); ++i) {
        int alertLevel = 0;
        cv::Mat frame;
        if (cameras[i].read(frame) && !frame.empty()) {

            // Convert the frame from BGR to RGB
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);


            // Object detection
            std::vector<cv::Rect> detections;
            if (usingHog) {
                pedestrianHOG.detectMultiScale(frame, detections);
            } else {
                faceCascade.detectMultiScale(frame, detections, 1.1, 3, 0, cv::Size(125, 125));
            }


            // Draw rectangles around detected objects, and manage logic of detected objects
            if (detections.empty()) {
                alertLevel = 0;
            } else {
                for (const auto& detected : detections) {
                    cv::rectangle(frame, detected, cv::Scalar(255, 0, 0), 2); // Draw a red rectangle

                    std::pair<int, int> position = {detected.x, detected.y};

                    QString currentId = objects.updateObject(i, position);

                    alertLevel = (objects.checkAlert(currentId)) ? 2 : 1;
                }
            }


            // Alert
            displayAlert(alertLevel, i);

            // Convert cv::Mat to QImage
            QImage image(
                frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);


            // Display the camera name
            cameraNameLabels[i]->setText(QString("CAM%1").arg(i));


            // Set the image to the QLabel
            cameraLabels[i]->setPixmap(QPixmap::fromImage(image).scaled(
                cameraLabels[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    objects.removePastObjects();
}
