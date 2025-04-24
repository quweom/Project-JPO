#pragma once
#include <QString>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVector>
#include <QColor>

class AirQualityIndex {
public:

    explicit AirQualityIndex(int index = 0) : m_index(index) {}

    //poziom indeksu
    struct IndexLevel {
        int id;                //0-5
        QString name;          //"Bardzo dobry"
    };

    //dane stanowiska
    struct StationData {
        QString paramName;     //"PM10"
        IndexLevel level;      //indeks parametru
        QDateTime calcDate;    //data obliczenia
    };

    AirQualityIndex(const QJsonObject &json);

    //funckje "get" parametrow
    int stationId() const { return m_stationId; }
    QDateTime calculationDate() const { return m_calcDate; }
    IndexLevel overallIndex() const { return m_overallIndex; }
    QDateTime sourceDataDate() const { return m_sourceDataDate; }
    const QVector<StationData>& stationReadings() const { return m_stationReadings; }

    QColor getQualityColor() const;

    //metody pomocnicze
    QString toString() const;
    bool isValid() const;

private:
    // m - member variable
    int m_index;
    int m_stationId;
    QDateTime m_calcDate;
    IndexLevel m_overallIndex;
    QDateTime m_sourceDataDate;
    QVector<StationData> m_stationReadings;
};
