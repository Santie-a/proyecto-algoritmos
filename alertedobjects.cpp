#include "alertedobjects.h"

void alertedObjects::saveAlerts(QString filename) {
    QJsonArray jsonArray;

    // Iterar sobre el contenedor y agregar cada elemento al JSON
    for (auto it = alertedContainer.cbegin(); it != alertedContainer.cend(); ++it) {
        QJsonObject jsonObject;
        jsonObject["id"] = it.key(); // Agregar la llave como un campo
        jsonObject["imgPath"] = it.value().imgPath;
        jsonObject["date"] = it.value().date.toString(Qt::ISODate);
        jsonObject["hour"] = it.value().hour.toString(Qt::ISODate);
        jsonObject["camera"] = it.value().camera;

        jsonArray.append(jsonObject);
    }

    // Crear el documento JSON y guardarlo en el archivo
    QJsonDocument jsonDoc(jsonArray);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qWarning() << "No se pudo guardar el archivo:" << filename;
    }
}

void alertedObjects::loadAlerts(QString filename) {
    QFile file(filename);

    // Verificar si el archivo existe y se puede abrir
    if (!file.exists()) {
        qWarning() << "El archivo no existe:" << filename;
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo:" << filename;
        return;
    }

    // Leer y parsear el archivo JSON
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isArray()) {
        qWarning() << "Formato de JSON inválido en:" << filename;
        return;
    }

    // Limpiar el contenedor actual
    alertedContainer.clear();

    QJsonArray jsonArray = jsonDoc.array();
    for (const QJsonValue &value : jsonArray) {
        if (!value.isObject()) {
            qWarning() << "Elemento no válido encontrado en JSON, omitiendo.";
            continue;
        }

        QJsonObject jsonObject = value.toObject();

        // Validar y extraer datos del JSON
        QString id = jsonObject["id"].toString();
        QString imgPath = jsonObject["imgPath"].toString();
        QDate date = QDate::fromString(jsonObject["date"].toString(), Qt::ISODate);
        QTime hour = QTime::fromString(jsonObject["hour"].toString(), Qt::ISODate);
        int camera = jsonObject["camera"].toInt(-1);

        if (id.isEmpty() || imgPath.isEmpty() || !date.isValid() || !hour.isValid() || camera == -1) {
            qWarning() << "Datos incompletos o inválidos en JSON, omitiendo entrada con id:" << id;
            continue;
        }

        // Insertar en el contenedor
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

void alertedObjects::insertAlerted(const QString &id, const QString &imgPath, const QDate &currentDate, const QTime &hour, int camera) {
    qDebug() << "Adding" << id << "to alerts...";
    alertedContainer[id] = alerted(imgPath, currentDate, hour, camera);
    qDebug() << "Container size:" << alertedContainer.size();
}

QList<alertedObjects::alerted> alertedObjects::getSortedByCamera() {
    QList<alerted> alertList = alertedContainer.values();
    std::sort(alertList.begin(), alertList.end(), [](const alerted &a, const alerted &b) {
        return a.camera < b.camera;
    });
    return alertList;
}

QList<alertedObjects::alerted> alertedObjects::getSortedByHour() {
    QList<alerted> alertList = alertedContainer.values();
    std::sort(alertList.begin(), alertList.end(), [](const alerted &a, const alerted &b) {
        return a.hour < b.hour;
    });
    return alertList;
}

QList<alertedObjects::alerted> alertedObjects::getSortedByDate() {
    QList<alerted> alertList = alertedContainer.values();
    std::sort(alertList.begin(), alertList.end(), [](const alerted &a, const alerted &b) {
        return a.date < b.date;
    });
    return alertList;
}
