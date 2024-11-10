#ifndef DETECTEDOBJECTS_H
#define DETECTEDOBJECTS_H

#include <QHash>
#include <QQueue>
#include <QString>
#include <QTime>

class detectedObjects
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
    QString retriveKey(int index, std::pair<int, int> &position);

    // Private helper functions
    bool isCloseTo(std::pair<int, int> &p1, std::pair<int, int> &p2);

public:
    detectedObjects();

    QString updateObject(int index, std::pair<int, int> &position);
    void removePastObjects();
    bool checkAlert(QString &id);
};

#endif // DETECTEDOBJECTS_H
