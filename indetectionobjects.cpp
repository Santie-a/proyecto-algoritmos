#include "indetectionobjects.h"

/**
 * Constructor for detectedObjects.
 * Initializes the detectedObjects class by printing a
 * debugging message and the initial size of the detectedContainer.
 */
inDetectionObjects::inDetectionObjects() {
    qDebug() << "Initializing detectedObjects...";
}

/**
 * Adds an object to the hash.
 * @param id The identifier for the object, which
 * will be used as the key in the hash.
 * @param initialPosition The initial position of the object.
 * This is used to initialize the head of the object's
 * position queue.
 */
void inDetectionObjects::addObject(QString &id, std::pair<int, int> &initialPosition) {
    qDebug() << "Trying to add object to hash...";
    detectedContainer.insert(id, detected(initialPosition));

    qDebug() << "Added" << id << "with initialPosition of x:" << initialPosition.first << "y:" << initialPosition.second;

    detected det = detectedContainer[id];
    qDebug() << "Time:" << det.startingTime;
}

/**
 * Checks if two positions are within a specified tolerance.
 *
 * @param p1 The first position as a pair of coordinates (x, y).
 * @param p2 The second position as a pair of coordinates (x, y).
 * @return True if the positions are within the tolerance range, false otherwise.
 */
bool inDetectionObjects::isCloseTo(std::pair<int, int> &p1, std::pair<int, int> &p2) {
    if (
        (p1.first <= p2.first + tolerance) &&
        (p1.first >= p2.first - tolerance) &&
        (p1.second <= p2.second + tolerance) &&
        (p1.second >= p2.second - tolerance)
        ) {
        return true;
    } else {
        return false;
    }
}


/**
 * Retrieves the key for the object at the specified position and time.
 * If the detectedContainer is empty or no close match is found, a new
 * key is generated based on the current time and camera index.
 *
 * @param index The index of the camera.
 * @param position The position of the object as a pair of coordinates (x, y).
 * @param currentTime The current time used for generating a new key if needed.
 * @return The key for the object, either an existing one or a new one.
 */
QString inDetectionObjects::retriveKey(int index, std::pair<int, int> &position, QTime &currentTime) {
    QString id;

    // Check if the detectedContainer is empty to create a new key
    if (detectedContainer.isEmpty()) {
        id = QString("CAM%1-%2-%3-%4").arg(index).arg(currentTime.hour()).arg(currentTime.minute()).arg(currentTime.second());
    } else {
        // Check if the detectedContainer contains a close match, doing so by checking the head of the positions queue
        const auto keys = detectedContainer.keys();
        for (const QString &key : keys) {
            detected &det = detectedContainer[key];

            if (det.positions && !det.positions->isEmpty()) {
                std::pair<int, int> keyStartingPos = det.positions->head();

                if(isCloseTo(position, keyStartingPos)) {
                    id = key;
                    break;
                }
            }
        }
    }

    // If no close match is found, create a new key (default)
    if (id == "") {
        id = QString("CAM%1-%2-%3-%4").arg(index).arg(currentTime.hour()).arg(currentTime.minute()).arg(currentTime.second());
    }

    return id;
}

/**
 * Updates the object at the specified index with the new position. If the object does not exist, creates a new one.
 * @param index The index of the camera.
 * @param position The new position of the object as a pair of coordinates (x, y).
 * @param currentTime The current time used for adding new positions to the queue.
 * @return The key for the object.
 */
QString inDetectionObjects::updateObject(int index, std::pair<int, int> &position, QTime &currentTime) {
    QString id = retriveKey(index, position, currentTime);

    // If the detectedContainer is empty, add the object
    if (detectedContainer.isEmpty()) {
        addObject(id, position);
    } else {
        // If the container does not contain the object, add it
        if (!detectedContainer.contains(id)) {
            addObject(id, position);
        } else {
            // Otherwise, update the object
            detected &det = detectedContainer[id];
            if (det.positions && (det.startingTime.secsTo(currentTime) > 1)) {
                det.positions->enqueue(position);
                det.lastInsertionTime = currentTime;
            }
        }
    }

    return id;
}

/**
 * Checks if an alert condition is met for the specified object.
 * An alert is triggered if the time difference between the object's
 * starting time and its last insertion time exceeds a predefined threshold.
 * 
 * @param id The identifier of the object in the detectedContainer.
 * @return True if the alert condition is met, false otherwise.
 */
bool inDetectionObjects::checkAlert(QString &id) {
    bool isAlert = false;

    detected &det = detectedContainer[id];

    int difference = det.startingTime.secsTo(det.lastInsertionTime); // Time from starting to last insertion

    if (difference > 10) {
        isAlert = true;
    }

    return isAlert;
}

/**
 * Removes objects from the hash if they have not been updated within the last 5 seconds.
 * This is used to clean up the hash and remove objects that are no longer being tracked.
 * @param currentTime The current time used for determining the age of objects.
 */
void inDetectionObjects::removePastObjects(QTime &currentTime) {
    if (!detectedContainer.isEmpty()) {
        const auto keys = detectedContainer.keys();
        for (const QString &key : keys) {
            detected &det = detectedContainer[key];
            int lastToCurrentTime = det.lastInsertionTime.secsTo(currentTime);

            if (lastToCurrentTime > 5) {
                qDebug() << "Deleting" << key << "because it past 5 seconds since last insertion...";

                detectedContainer.remove(key);

                qDebug() << "Container size after deletion:" << detectedContainer.size();
            }
        }
    }
}
