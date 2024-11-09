#include "detectedobjects.h"

detectedObjects::detectedObjects() {
    qDebug() << "Initializing detectedObjects...";
    qDebug() << "Initial size of detectedContainer:" << detectedContainer.size();
}

void detectedObjects::addObject(QString &id, std::pair<int, int> &initialPosition) {
    qDebug() << "Trying to add object to hash...";
    detectedContainer.insert(id, detected(initialPosition));

    qDebug() << "Added" << id << "with initialPosition of x:" << initialPosition.first << "y:" << initialPosition.second;

    detected det = detectedContainer[id];
    qDebug() << "Time:" << det.startingTime.msec();
}

bool detectedObjects::isCloseTo(std::pair<int, int> &p1, std::pair<int, int> &p2) {
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

QString detectedObjects::retriveKey(int index, std::pair<int, int> &position) {
    QString id;

    if (detectedContainer.isEmpty()) {
        id = QString("CAM%1-%2-%3").arg(index).arg(position.first).arg(position.second);
    } else {
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

    if (id == "") {
        id = QString("CAM%1-%2-%3").arg(index).arg(position.first).arg(position.second);
    }

    return id;
}

void detectedObjects::updateObjects(int index, std::pair<int, int> &position) {
    QString id = retriveKey(index, position);

    if (detectedContainer.isEmpty()) {
        addObject(id, position);
    } else {
        if (!detectedContainer.contains(id)) {
            addObject(id, position);
        } else {
            detected &det = detectedContainer[id];
            if (det.positions) {
                det.positions->enqueue(position);
            }
        }
    }
}
