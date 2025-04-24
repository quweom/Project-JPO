#pragma once
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QJsonObject>

class Measurement {
public:
    //struktura dla pojedynczego pomiaru
    struct DataPoint {
        QDateTime timestamp;
        double value; //wartość może być nan dla brakujących pomiarów
    };

    Measurement(const QJsonObject &json);

    //get
    QString paramCode() const { return m_paramCode; }
    const QVector<DataPoint>& data() const { return m_data; }

    //metody pomocnicze
    bool isEmpty() const { return m_data.empty(); }
    QString toString() const;

    //analiza danych
    double maxValue() const;
    double minValue() const;
    QDateTime dateOfMaxValue() const;

private:
    // m - member variable
    QString m_paramCode;
    QVector<DataPoint> m_data;
};
