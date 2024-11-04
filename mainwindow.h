#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrames();

private:
    // Windows and UI
    Ui::MainWindow ui;
    QGridLayout *gridLayout;

    // Vectors to store the objects

    // *Camera
    QVector<cv::VideoCapture> cameras;

    // *Text
    QVector<QLabel*> cameraLabels;
    QVector<QLabel*> fpsLabels;
    QVector<QLabel*> objectsLabels;

    // Extra
    QTimer *timer;
    cv::CascadeClassifier faceCascade;

    // Funciones organizadoras
    void createUI();
    void setCameras();
    void loadCascade();
};
#endif // MAINWINDOW_H
