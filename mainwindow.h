#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QGridLayout>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createUI();
    void setCameras();
    void loadCascade();
    void updateFrames();

    QTimer *timer;
    QGridLayout *gridLayout;
    QList<QLabel*> cameraLabels;
    QList<QLabel*> fpsLabels;
    QList<QLabel*> objectsLabels;
    QVector<cv::VideoCapture> cameras;
    cv::CascadeClassifier faceCascade;
};

#endif // MAINWINDOW_H