#ifndef ALERTEDOBJECTS_H
#define ALERTEDOBJECTS_H

#include <QString>
#include <QMap>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

// Class to load and store the alerted objects, with sorter f
class alertedObjects {
public:
    // Struct for storing alert
    struct alerted {
        QString imgPath;
        QDate date;
        QTime hour;
        int camera;

        // Default constructor
        alerted() : imgPath(""), date(QDate::currentDate()), hour(QTime::currentTime()), camera(-1) {}

        // Initial value constructor
        alerted(const QString &_imgPath, const QDate &_date, const QTime &_hour, int _camera)
            : imgPath(_imgPath), date(_date), hour(_hour), camera(_camera) {}
    };

    // Save alerts
    void saveAlerts(QString filename);

    // Load alerts
    void loadAlerts(QString filename);

    // Insert alert
    void insertAlerted(const QString &id, const QString &imgPath, const QDate &currentDate, const QTime &hour, int camera);

    // Sorter funcions
    QList<alerted> getSortedByCamera();
    QList<alerted> getSortedByDate();
    QList<alerted> getSortedByHour();

    // Operation functions
    alerted operator[](QString key);
    bool contains(QString &key);

private:
    // Container
    QMap<QString, alerted> alertedContainer; // Mapa que almacena IDs y sus detecciones
};

#endif // ALERTEDOBJECTS_H
