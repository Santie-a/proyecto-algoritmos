#include "alertedobjects.h"

/**
 * Saves the current alerted objects to a JSON file.
 * 
 * This function iterates over the alerted objects container, converting each
 * alerted object into a JSON object with fields for id, imgPath, date, hour,
 * and camera. The JSON objects are added to a JSON array, which is then 
 * serialized to a JSON document and saved to the specified file. If the file
 * cannot be opened for writing, a warning message is logged.
 * 
 * @param filename The name of the file where the JSON data will be saved.
 */
void alertedObjects::saveAlerts(QString filename) {
    QJsonArray jsonArray;

    // Iterate over the container and convert each alerted object to a JSON object
    for (auto it = alertedContainer.cbegin(); it != alertedContainer.cend(); ++it) {
        QJsonObject jsonObject;
        jsonObject["id"] = it.key(); // Agregar la llave como un campo
        jsonObject["imgPath"] = it.value().imgPath;
        jsonObject["date"] = it.value().date.toString(Qt::ISODate);
        jsonObject["hour"] = it.value().hour.toString(Qt::ISODate);
        jsonObject["camera"] = it.value().camera;

        jsonArray.append(jsonObject);
    }

    // Create and save the JSON document
    QJsonDocument jsonDoc(jsonArray);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qWarning() << "No se pudo guardar el archivo:" << filename;
    }
}

/**
 * Loads alerted objects from a JSON file and populates the alertedContainer.
 * 
 * This function checks if the specified JSON file exists and can be opened for reading.
 * It reads and parses the JSON data, expecting an array of JSON objects with fields
 * "id", "imgPath", "date", "hour", and "camera". Each valid JSON object is converted
 * to an `alerted` object and inserted into the `alertedContainer`. If any JSON object
 * contains invalid or incomplete data, it is skipped, and a warning message is logged.
 * The current contents of the `alertedContainer` are cleared before loading new data.
 * 
 * @param filename The name of the file from which to load the JSON data.
 */
void alertedObjects::loadAlerts(QString filename) {
    QFile file(filename);

    // Check if the file exists
    if (!file.exists()) {
        qWarning() << "El archivo no existe:" << filename;
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo:" << filename;
        return;
    }

    // Read the JSON data
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isArray()) {
        qWarning() << "Formato de JSON inválido en:" << filename;
        return;
    }

    // Clear the current container
    alertedContainer.clear();

    QJsonArray jsonArray = jsonDoc.array();
    for (const QJsonValue &value : jsonArray) {
        if (!value.isObject()) {
            qWarning() << "Elemento no válido encontrado en JSON, omitiendo.";
            continue;
        }

        QJsonObject jsonObject = value.toObject();

        // Validate the JSON object
        QString id = jsonObject["id"].toString();
        QString imgPath = jsonObject["imgPath"].toString();
        QDate date = QDate::fromString(jsonObject["date"].toString(), Qt::ISODate);
        QTime hour = QTime::fromString(jsonObject["hour"].toString(), Qt::ISODate);
        int camera = jsonObject["camera"].toInt(-1);

        if (id.isEmpty() || imgPath.isEmpty() || !date.isValid() || !hour.isValid() || camera == -1) {
            qWarning() << "Datos incompletos o inválidos en JSON, omitiendo entrada con id:" << id;
            continue;
        }

        // Insert the valid JSON object into the container
        alertedContainer.insert(id, alerted(imgPath, date, hour, camera));
    }

    for (auto it = alertedContainer.cbegin(); it != alertedContainer.cend(); ++it) {
        qDebug() << "ID:" << it.key()
        << "Image Path:" << it.value().imgPath
        << "Date:" << it.value().date.toString("yyyy-MM-dd")
        << "Hour:" << it.value().hour.toString("HH:mm:ss")
        << "Camera:" << it.value().camera;
    }
}

/**
 * Inserts a new alerted object into the container.
 * 
 * This function adds a new entry to the alertedContainer with the specified
 * id, imgPath, currentDate, hour, and camera. If the id already exists in the
 * container, its associated alerted object will be updated.
 * @param id The identifier for the new alerted object.
 * @param imgPath The path to the image associated with the alert.
 * @param currentDate The current date.
 * @param hour The current time.
 * @param camera The number of the camera where the alert was detected.
 */
void alertedObjects::insertAlerted(const QString &id, const QString &imgPath, const QDate &currentDate, const QTime &hour, int camera) {
    qDebug() << "Adding" << id << "to alerts...";
    alertedContainer[id] = alerted(imgPath, currentDate, hour, camera);
    qDebug() << "Container size:" << alertedContainer.size();
}

/**
 * Returns a sorted list of alerted objects based on the camera number.
 * The list is sorted in ascending order by camera number.
 * @return A sorted list of alerted objects.
 */
QList<alertedObjects::alerted> alertedObjects::getSortedByCamera() {
    QList<alerted> alertList = alertedContainer.values();
    std::sort(alertList.begin(), alertList.end(), [](const alerted &a, const alerted &b) {
        return a.camera < b.camera;
    });
    return alertList;
}

/**
 * Returns a sorted list of alerted objects based on the time of day.
 * The list is sorted in ascending order by time of day.
 * @return A sorted list of alerted objects.
 */
QList<alertedObjects::alerted> alertedObjects::getSortedByHour() {
    QList<alerted> alertList = alertedContainer.values();
    std::sort(alertList.begin(), alertList.end(), [](const alerted &a, const alerted &b) {
        return a.hour < b.hour;
    });
    return alertList;
}

/**
 * Returns a sorted list of alerted objects based on the date.
 * The list is sorted in ascending order by date.
 * @return A sorted list of alerted objects.
 */
QList<alertedObjects::alerted> alertedObjects::getSortedByDate() {
    QList<alerted> alertList = alertedContainer.values();
    std::sort(alertList.begin(), alertList.end(), [](const alerted &a, const alerted &b) {
        return a.date < b.date;
    });
    return alertList;
}

/**
 * Overloaded operator to access an alerted object by its id.
 * @param key The id of the alerted object.
 * @return The alerted object associated with the given id.
 */
alertedObjects::alerted alertedObjects::operator[](QString key) {
    const alertedObjects::alerted obj = alertedContainer[key];
    return obj;
}

/**
 * Checks if the alertedContainer contains an alerted object with the given key.
 * @param key The id of the alerted object to search for.
 * @return True if the alertedContainer contains an alerted object with the given key, false otherwise.
 */

bool alertedObjects::contains(QString &key) {
    return alertedContainer.contains(key);
}
