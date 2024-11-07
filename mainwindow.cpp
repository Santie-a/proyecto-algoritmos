#include "MainWindow.h"
#include <QImage>
#include <QDebug>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(800, 600);

    createUI();

    loadCascade();

    setCameras();

    // Set up a timer to update frames
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrames);
    timer->start(30);  // Update every 30 ms (~33 FPS)
}

MainWindow::~MainWindow() {
    // Release all camera resources
    for (auto &camera : cameras) {
        if (camera.isOpened()) {
            camera.release();
        }
    }
}

void MainWindow::loadCascade() {
    // Load the Haar Cascade classifier
    if (!faceCascade.load("E:/dev/opencvsource/opencv-4.10.0/data/haarcascades/haarcascade_frontalface_default.xml")) { // Make sure to provide the correct path
        qDebug() << "Error loading face cascade classifier.";
        return;
    }
}

void MainWindow::createUI() {
    // Creating the containers
    QWidget *centralWidget = new QWidget(this);

    // Main container
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Title creation and styling
    QWidget *headerWidget = new QWidget(this); // Title container
    headerWidget->setStyleSheet("background-color: #2E86C1; padding: 10px; border-radius: 3px;");

    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget); // Title space

    QLabel *titleLabel = new QLabel("Camera Viewer", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

    headerLayout->addWidget(titleLabel); // Adding the title space to the container
    mainLayout->addWidget(headerWidget); // Adding the title container to the main container

    // Add a spacer to push other content down
    QSpacerItem *topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addItem(topSpacer);

    // Cameras space
    gridLayout = new QGridLayout(centralWidget);
    gridLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(gridLayout);

    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(bottomSpacer);

    setCentralWidget(centralWidget);
    setMinimumSize(800, 600);
    setMaximumSize(1000, 800);
}

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

        // Create a QLabel to siplay the name
        QLabel *cameraNameLabel = new QLabel(this);
        cameraNameLabels.append(cameraNameLabel);
        gridLayout->addWidget(cameraNameLabel, row, col);

        // Create a QLabel to display the camera feed
        QLabel *cameraLabel = new QLabel(this);
        cameraLabel->setMinimumSize(minWidth, minHeight);
        cameraLabel->setAlignment(Qt::AlignCenter);
        cameraLabels.append(cameraLabel);
        gridLayout->addWidget(cameraLabel, row + 1, col);

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

void MainWindow::displayAlert(bool val, int index) {
    if (val) {
        cameraNameLabels[index]->setStyleSheet("background-color: yellow; font-weight: bold; font-size: 30px; padding: 5px;");
    } else {
        cameraNameLabels[index]->setStyleSheet("background-color: transparent; font-weight: bold; font-size: 30px; padding: 5px;");
    }
}

void MainWindow::updateFrames() {
    for (int i = 0; i < cameras.size(); ++i) {
        cv::Mat frame;
        if (cameras[i].read(frame) && !frame.empty()) {

            // Convert the frame from BGR to RGB
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);


            // Face detection
            std::vector<cv::Rect> faces;
            faceCascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(75, 75));


            // Draw rectangles around detected faces
            for (const auto& face : faces) {
                cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2); // Draw a red rectangle
                qDebug() << "x: " << face.x << " y: " << face.y;
            }


            // Alert
            displayAlert(faces.size() > 0, i);

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
}
