#include "MainWindow.h"

/**
 * Constructor for MainWindow.
 * Initializes the main window with a size of 1000x800.
 * Calls the setup functions for the UI, cascade, camera, and timer.
 * @param parent The parent QWidget for this window.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(1000, 800);

    createUI();

    loadDetector(false);

    alerts.loadAlerts("../../data/alerts.json");
    updateAlertedList(alerts.getSortedByDate());

    // Connections for interactivity
    connect(alertsWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemClicked);
    connect(comboBoxSortOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(onSortOptionChanged(int)));

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
 * Loads the Haar Cascade and SVM classifier for face detection or pedestrian detection from the provided path.
 * If the load fails, prints an error message to the console.
 */
void MainWindow::loadDetector(bool pedestrian) {
    // Load the Haar Cascade classifier or the SVM classifier
    if (pedestrian) {
        pedestrianHOG.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
        usingHog = true;
    }
    else {
        if (!faceCascade.load("../../cascades/haarcascade_frontalface_default.xml")) { // Two orders above build folder
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
    sidebarWidget = new QListWidget(this);
    sidebarWidget->setMaximumWidth(250);
    sidebarWidget->setStyleSheet("background-color: #121212; padding: 3px; margin: 5px;");
    sidebarLayout = new QVBoxLayout(sidebarWidget);

    // *Menu title
    QLabel *menuLabel = new QLabel("Registro", this);
    menuLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold; padding: 5px");
    sidebarLayout->addWidget(menuLabel); // Adding the title space to the container

    // *Sorting options
    comboBoxSortOptions = new QComboBox(this);
    comboBoxSortOptions->addItem("Fecha");
    comboBoxSortOptions->addItem("Hora");
    comboBoxSortOptions->addItem("Cámara");
    comboBoxSortOptions->setStyleSheet("background-color: #808080");

    sidebarLayout->addWidget(comboBoxSortOptions);

    // *Log space
    alertsWidget = new QListWidget(this);
    sidebarLayout->addWidget(alertsWidget);

    // Content container
    contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignCenter);
    centralLayout->addLayout(contentLayout);

    // Title creation and styling
    headerWidget = new QWidget(this); // Title container
    headerWidget->setStyleSheet("background-color: #121212; padding: 10px; margin: 5px;");

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
    sidebarLayout->addItem(bottomSpacer);

    centralLayout->addWidget(sidebarWidget);

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

    alertLevelsAndTimes.resize(cameraCount);
    alertLevelsAndTimes.fill({0, QTime::currentTime()});


    for (int i = 0; i < cameraCount; i++) {
        cv::VideoCapture cap(cameraIndex);

        if (!cap.isOpened()) {
            qDebug() << "Camera" << cameraIndex << "not available.";
            break;
        }

        // Create a QLabel to display the name in the grid
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

void MainWindow::displayImage(QString &imgPath) {
    qDebug() << "Trying to display img" << imgPath;
    QDialog dialog(this);
    QVBoxLayout layout(&dialog);

    QLabel imageLabel;
    QPixmap pixmap(imgPath);
    if (pixmap.isNull()) {
        imageLabel.setText("Failed to load image.");
    } else {
        imageLabel.setPixmap(pixmap.scaled(600, 400, Qt::KeepAspectRatio)); // Scale the image
    }

    layout.addWidget(&imageLabel);
    dialog.setLayout(&layout);
    dialog.exec(); // Show the dialog
}

/**
 * Slot connected to the combo box that changes the sorting order of the alerts list.
 * @param index The index of the selected item in the combo box, which corresponds to the sorting order.
 */
void MainWindow::onSortOptionChanged(int index) {
    QList<alertedObjects::alerted> sortedList;


    switch (index) {
    case 0: // "Sort by Camera"
        sortedList = alerts.getSortedByDate();
        break;
    case 1: // "Sort by Hour"
        sortedList = alerts.getSortedByHour();
        break;
    case 2: // "Sort by Date"
        sortedList = alerts.getSortedByCamera();
        break;
    }

    // Call the updateAlertedList function with the sorted list
    updateAlertedList(sortedList);
}

/**
 * Slot triggered when an item in the alerts list is clicked.
 * Displays an image dialog with the image corresponding to the clicked item.
 * @param item The item that was clicked.
 */
void MainWindow::onItemClicked(QListWidgetItem *item) {
    // Get the path of the image associated with the clicked item
    QString imgPath = item->data(Qt::UserRole).toString();

    // Call the displayImage function to display the image
    displayImage(imgPath);
}

/**
 * Updates the alerts list widget with the given list of alerts.
 * 
 * This function clears the current list and then iterates over the given list of alerts,
 * adding each alert as a new item in the list. The item's text is set to a string containing
 * the camera number, date, and time of the alert. The item's background and foreground colors
 * are set to a dark gray and light gray, respectively. The item's data is set to the path of
 * the image associated with the alert, which can be accessed later if needed.
 * @param alertedList The list of alerts to display in the list widget
 */
void MainWindow::updateAlertedList(QList<alertedObjects::alerted> alertedList) {
    // Clear the current list
    alertsWidget->clear();

    // Iterate over the list of alerts
    for (const alertedObjects::alerted &alert : alertedList) {
        // Create a formatted string with the camera number, date, and time
        QString alertInfo = QString("CAM%1 - %2 - %3").arg(alert.camera).arg(alert.date.toString("yyyy-MM-dd"), alert.hour.toString());

        // Create a new QListWidgetItem with the formatted string
        QListWidgetItem *item = new QListWidgetItem(alertInfo);
        item->setBackground(QColor(60, 60, 60));
        item->setForeground(QColor(240, 240, 240));

        // Set the user role of the item to the path of the image associated with the alert
        item->setData(Qt::UserRole, alert.imgPath);

        // Add the item to the list
        alertsWidget->addItem(item);
    }
}


/**
 * Updates the frames of all cameras and performs object detection.
 * If an object is detected, it will draw a red rectangle around it and
 * update the alert level and time if the object is not already being tracked.
 * If the object has been detected for more than 2 seconds, it will save an image
 * to the data/img directory and add the alert to the alerts list.
 * It will then update the alert list widget with the new alert.
 * Finally, it will call the removePastObjects function to clean up the hash of detected objects.
 * @see inDetectionObjects::removePastObjects
 */
void MainWindow::updateFrames() {
    QTime currentTime = QTime::currentTime();
    QDate currentDate = QDate::currentDate();

    for (int i = 0; i < cameras.size(); ++i) {
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
                alertLevelsAndTimes[i].first = 0;
            } else {
                for (const auto& detected : detections) {
                    cv::rectangle(frame, detected, cv::Scalar(255, 0, 0), 2); // Draw a red rectangle

                    std::pair<int, int> position = {detected.x, detected.y}; // Position of the object

                    QString currentId = objects.updateObject(i, position, currentTime); // Update the object (class inDetectionObjects)

                    // Check if the object has been detected for more than 2 seconds
                    if (alertLevelsAndTimes[i].second.secsTo(currentTime) > 2) {
                        // Check if the object is already being tracked (class inDetectionObjects)
                        if (objects.checkAlert(currentId)) {

                            alertLevelsAndTimes[i].first = 2; // Set the alert level to 2
                            QString imgPath = QString("../../data/img/%1.png").arg(currentId);

                            // Save the image
                            cv::Mat rgbFrame;
                            cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
                            cv::imwrite(imgPath.toStdString(), rgbFrame);

                            // Add the alert (class alertedObjects)
                            alerts.insertAlerted(currentId, imgPath, currentDate, currentTime, i);
                            alertLevelsAndTimes[i].second = QTime::currentTime();

                            // Update the alert list widget with the current sorting method
                            onSortOptionChanged(comboBoxSortOptions->currentIndex());

                        } else {
                            // Otherwise, set the alert level to 1 (no alert, only detection)
                            alertLevelsAndTimes[i].first = 1;
                            alertLevelsAndTimes[i].second = QTime::currentTime();
                        }
                    }
                }
            }


            // Alert
            displayAlert(alertLevelsAndTimes[i].first, i);

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
    objects.removePastObjects(currentTime);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Show a confirmation dialog when the user tries to close the application
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Cerrar aplicación",
                                                               "¿Estás seguro de que deseas salir?\nSe guardarán los datos automáticamente.",
                                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    // If the user clicks "Yes", save the alerts and accept the close event
    if (resBtn == QMessageBox::Yes) {
        alerts.saveAlerts("../../data/alerts.json");
        event->accept();
    } else {
        event->ignore();
    }
}
