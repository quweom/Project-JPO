#pragma once
#include <QString>
#include <QJsonObject>

class Station {
public:
    //struktura dla danych adresowych
    struct Address {
        int cityId;
        QString cityName;
        QString communeName;
        QString districtName;
        QString provinceName;
        QString streetName;
    };

    Station(const QJsonObject &json);

    //get
    int id() const { return m_id; }
    QString name() const { return m_name; }
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    Address address() const { return m_address; }

    //metoda pomocnicza do debugowania
    QString toString() const;

    bool isInCity(const QString& city) const;
    double distanceTo(double lat, double lon) const;
    QString toShortString() const;  //krótki opis do listy
    QString toFullString() const;   //pełne dane

    //get do nowych funkcjonalności
    QString cityName() const { return m_address.cityName; }



private:
    // m - member variable
    int m_id;
    QString m_name;
    double m_latitude;
    double m_longitude;
    Address m_address;
};
