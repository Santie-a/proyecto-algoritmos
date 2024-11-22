#ifndef INDETECTIONDOBJECTS_H
#define INDETECTIONDOBJECTS_H

#include <QHash>
#include <QQueue>
#include <QString>
#include <QTime>

class inDetectionObjects
{
private:
    // Struct for the detected object
    struct detected
    {
        QQueue<std::pair<int, int>> *positions;
        QTime startingTime;
        QTime lastInsertionTime;

        // Default constructor
        detected() : positions(new QQueue<std::pair<int, int>>), startingTime(QTime::currentTime()), lastInsertionTime(QTime::currentTime()) {}

        // Constructor with initial position
        detected(std::pair<int, int> &initialPosition)
            : positions(new QQueue<std::pair<int, int>>), startingTime(QTime::currentTime()), lastInsertionTime(QTime::currentTime()) {
            positions->enqueue(initialPosition);
        }
    };
    // Tolerance for retrieving the key
    int tolerance = 50;

    // Hash for tracking the objects (main container)
    QHash<QString, detected> detectedContainer;

    // Private main functions
    void addObject(QString &id, std::pair<int, int> &initialPosition);
    QString retriveKey(int index, std::pair<int, int> &position, QTime &currentTime);

    // Private helper functions
    bool isCloseTo(std::pair<int, int> &p1, std::pair<int, int> &p2);

public:
    inDetectionObjects();

    QString updateObject(int index, std::pair<int, int> &position, QTime &currentTime);
    void removePastObjects(QTime &currentTime);
    bool checkAlert(QString &id);
};

#endif // INDETECTEDOBJECTS_H
