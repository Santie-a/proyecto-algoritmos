#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "indetectionobjects.h"
#include "alertedobjects.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QQueue>
#include <QHash>
#include <QComboBox>
#include <QImage>
#include <QDebug>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QDateTime>
#include <QMessageBox>
#include <QCloseEvent>
#include <QListWidget>

#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateFrames();

public slots:
    void onSortOptionChanged(int index);

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
    QListWidget *sidebarWidget;
    QListWidget *alertsWidget;

    // Class instance to store the objects that are being detected
    inDetectionObjects objects;

    // Class instance to store that have been detected
    alertedObjects alerts;
    QList<std::pair<int, QTime>> alertLevelsAndTimes;
    QComboBox *comboBoxSortOptions;

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

    // Organization functions
    void createUI();
    void setCameras();
    void loadDetector(bool pedestrian);
    void displayAlert(int val, int index);
    void closeEvent(QCloseEvent *event);

    // Helper fuctions
    void updateAlertedList(QList<alertedObjects::alerted>);
    void onItemClicked(QListWidgetItem *item);
    void displayImage(QString &imgPath);

};
#endif // MAINWINDOW_H
