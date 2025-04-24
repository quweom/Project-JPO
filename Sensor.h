#pragma once
#include <QString>
#include <QJsonObject>

class Sensor {
public:
    //struktura dla parametru pomiarowego
    struct Param {
        QString name;
        QString formula;
        QString code;
        int id;
    };

    Sensor(const QJsonObject &json);

    //get
    int id() const { return m_id; }
    int stationId() const { return m_stationId; }
    Param parameter() const { return m_param; }

    //metoda pomocnicza
    QString toString() const;

private:
    // m - member variable
    int m_id;
    int m_stationId;
    Param m_param;
};
