#include "mainwindow.h"
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
    // For macOS, assuming OpenCV is installed via Homebrew
    const QString cascadePath = "/opt/homebrew/Cellar/opencv/4.10.0_12/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";

    if (!faceCascade.load(cascadePath.toStdString())) {
        qDebug() << "Error loading face cascade classifier from:" << cascadePath;
        return;
    }
}

void MainWindow::createUI() {
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create main layout for central widget
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Header widget
    QWidget *headerWidget = new QWidget;
    headerWidget->setStyleSheet("background-color: #2E86C1; padding: 10px; border-radius: 3px;");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);

    // Title label
    QLabel *titleLabel = new QLabel("Camera Viewer");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    headerLayout->addWidget(titleLabel);

    // Add header to main layout
    mainLayout->addWidget(headerWidget);

    // Add spacing
    mainLayout->addSpacing(20);

    // Create grid layout for cameras
    QWidget *camerasWidget = new QWidget;
    gridLayout = new QGridLayout(camerasWidget);
    gridLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(camerasWidget);

    // Add bottom spacing
    mainLayout->addSpacing(20);

    // Set window properties
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

        // Create a QLabel to display the camera feed
        QLabel *cameraLabel = new QLabel(this);
        cameraLabel->setMinimumSize(minWidth, minHeight);
        cameraLabel->setAlignment(Qt::AlignCenter);
        cameraLabels.append(cameraLabel);
        gridLayout->addWidget(cameraLabel, row, col);

        // Create a QLabel for displaying FPS
        QLabel *fpsLabel = new QLabel(this);
        fpsLabel->setAlignment(Qt::AlignCenter);
        fpsLabel->setStyleSheet("font-weight: bold;");
        fpsLabels.append(fpsLabel);
        gridLayout->addWidget(fpsLabel, row + 1, col);

        // Create a QLabel for displaying detected objects
        QLabel *objectsLabel = new QLabel(this);
        objectsLabel->setAlignment(Qt::AlignCenter);
        objectsLabel->setStyleSheet("font-weight: bold;");
        objectsLabels.append(objectsLabel);
        gridLayout->addWidget(objectsLabel, row + 2, col);

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

void MainWindow::updateFrames() {
    static QVector<QElapsedTimer> timers(cameras.size()); // Store timers for each camera

    for (int i = 0; i < cameras.size(); ++i) {
        cv::Mat frame;
        if (cameras[i].read(frame) && !frame.empty()) {
            // Convert the frame from BGR to RGB
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

            // Face detection
            std::vector<cv::Rect> faces;
            faceCascade.detectMultiScale(frame, faces, 1.1, 3, 0, cv::Size(30, 30));

            // Draw rectangles around detected faces
            for (const auto& face : faces) {
                cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2); // Draw a red rectangle
            }

            // Convert cv::Mat to QImage
            QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

            // Set the image to the QLabel
            cameraLabels[i]->setPixmap(QPixmap::fromImage(image).scaled(
                cameraLabels[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

            // Display the detected objects
            objectsLabels[i]->setText(QString("Detected Objects: %1").arg(faces.size()));

            // Calculate and update FPS
            if (!timers[i].isValid()) {
                timers[i].start(); // Start the timer on first frame
            } else {
                double elapsed = timers[i].elapsed(); // Elapsed time in milliseconds
                if (elapsed > 0) {
                    double fps = 1000.0 / elapsed; // Calculate FPS
                    fpsLabels[i]->setText(QString("FPS: %1").arg(fps, 0, 'f', 2)); // Update FPS label
                }
                timers[i].restart(); // Restart the timer for the next frame
            }
        }
    }
}
