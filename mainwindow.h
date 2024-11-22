#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "detectedobjects.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QQueue>
#include <QHash>
#include <QComboBox>

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

    // *Layouts
    QVBoxLayout *sidebarLayout;
    QVBoxLayout *contentLayout;
    QVBoxLayout *headerLayout;
    QGridLayout *gridLayout;

    // *Widgets
    QWidget *mainWidget;
    QWidget *centralWidget;
    QWidget *headerWidget;
    QWidget *sidebarWidget;

    // class instance to store the objects
    detectedObjects objects;
    int alertLevel;

    // *Camera
    QVector<cv::VideoCapture> cameras;

    // *Text
    QLabel *titleLabel;

    // **For the cameras
    QVector<QLabel*> cameraLabels;
    QVector<QLabel*> cameraNameLabels;
    QVector<QComboBox *> camerasOptions;

    // Extra
    QSpacerItem *topSpacer;
    QSpacerItem *bottomSpacer;
    QTimer *timer;

    // OpenCV
    cv::CascadeClassifier faceCascade;
    cv::HOGDescriptor pedestrianHOG;
    bool usingHog = false;

    // Funciones organizadoras
    void createUI();
    void setCameras();
    void loadDetector(bool pedestrian);
    void displayAlert(int val, int index);
};
#endif // MAINWINDOW_H
