#include "AirQualityIndex.h"
#include <QDebug>
#include <QColor>

AirQualityIndex::AirQualityIndex(const QJsonObject &json) {
    //parsowanie podstawowych pól
    m_stationId = json["id"].toInt();
    m_calcDate = QDateTime::fromString(json["stCalcDate"].toString(), Qt::ISODate);
    m_sourceDataDate = QDateTime::fromString(json["stSourceDataDate"].toString(), Qt::ISODate);

    //parsowanie głównego indeksu
    QJsonObject indexObj = json["stIndexLevel"].toObject();
    m_overallIndex.id = indexObj["id"].toInt();
    m_overallIndex.name = indexObj["indexLevelName"].toString();

    //parsowanie danych stanowisk pomiarowych
    QJsonArray stations = json["stations"].toArray();
    for (const QJsonValue &stationVal : stations) {
        QJsonObject stationObj = stationVal.toObject();
        StationData data;

        data.paramName = stationObj["paramName"].toString();
        data.calcDate = QDateTime::fromString(stationObj["calcDate"].toString(), Qt::ISODate);

        QJsonObject levelObj = stationObj["indexLevel"].toObject();
        data.level.id = levelObj["id"].toInt();
        data.level.name = levelObj["indexLevelName"].toString();

        m_stationReadings.append(data);
    }
}

QString AirQualityIndex::toString() const {
    QString result = QString("Wskaźnik jakości powietrza dla stacji: %1\n")
    .arg(m_stationId);

    result += QString("Indeks ogólny: %1 (%2)\n")
                  .arg(m_overallIndex.id)
                  .arg(m_overallIndex.name);

    result += QString("Obliczone: %1\n")
                  .arg(m_calcDate.toString("yyyy-MM-dd HH:mm"));

    result += "Odczyty stacji:\n";
    for (const StationData &data : m_stationReadings) {
        result += QString("- %1: %2 (%3)\n")
        .arg(data.paramName)
            .arg(data.level.id)
            .arg(data.level.name);
    }

    return result;
}

bool AirQualityIndex::isValid() const {
    return m_stationId > 0 && m_calcDate.isValid();
}

QColor AirQualityIndex::getQualityColor() const {
    if (!isValid()) return Qt::gray; // Dla danych nieważnych


    int indexValue = m_overallIndex.id;
    if (indexValue <= 50) return QColor(0, 228, 0);
    else if (indexValue <= 100) return QColor(255, 255, 0);
    else if (indexValue <= 150) return QColor(255, 126, 0);
    else if (indexValue <= 200) return QColor(255, 0, 0);
    else return QColor(153, 0, 76);                  //Niebezpieczny(do poprawy)
}
