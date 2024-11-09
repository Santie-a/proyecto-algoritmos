QT       += core gui
QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    detectedobjects.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    detectedobjects.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += E:\dev\opencv\build\include

LIBS += E:\dev\opencv-build\bin\libopencv_core4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_highgui4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_imgcodecs4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_imgproc4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_features2d4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_calib3d4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_videoio4100.dll
LIBS += E:\dev\opencv-build\bin\libopencv_objdetect4100.dll

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
