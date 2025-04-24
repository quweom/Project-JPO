#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include "Station.h"
#include "Sensor.h"
#include "Measurement.h"
#include "AirQualityIndex.h"

class ApiHandler : public QObject {
    Q_OBJECT

public:
    explicit ApiHandler(QObject *parent = nullptr);

    //główne metody API
    void fetchStations(); //lista stancji
    void fetchSensors(int stationId); //czujniki dla stancji
    void fetchMeasurements(int sensorId);//pomiary z czujnika
    void fetchAirQualityIndex(int stationId);//index jakosci poweitrza

    //metody pomocnicze
    void setApiUrl(const QString &url);//zmiana bazowego url
    bool isBusy() const;//sprawdzanie czy zapytanie jest w stanie wypelnienia

    void filterStationsByCity(const QString& city);
    void findStationsInRadius(double lat, double lon, double radiusKm);
    void findStationsByAddress(const QString& address, double radiusKm);


signals:
    //sygnały z wynikami (wyniki przez qt)
    void stationsFetched(const QVector<Station> &stations);
    void sensorsFetched(const QVector<Sensor> &sensors);
    void measurementsFetched(const Measurement &measurement);
    void airQualityIndexFetched(const AirQualityIndex &index);

    void stationsFiltered(const QVector<Station>& stations);

    void geocodingFinished(double latitude, double longitude);


    //sygnały błędów
    void networkError(const QString &message);
    void apiError(const QString &errorCode);
    void geocodingError(const QString& message);

private slots:

    //analiza json oraz emitacja sygnalow
    void handleStationsReply();
    void handleSensorsReply();
    void handleMeasurementsReply();
    void handleAirQualityIndexReply();
    void handleGeocodingReply(); // adres -> sygnaly
    void handleNetworkError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager m_manager;
    QString m_apiBaseUrl = "https://api.gios.gov.pl/pjp-api/rest";
    bool m_isBusy = false;
    QVector<Station> m_allStations; //cache wszystkich stacji
    QNetworkAccessManager m_geocoderManager;
    QNetworkRequest createRequest(const QUrl &url) const;


    //metody prywatne
    QUrl buildUrl(const QString &endpoint) const; // budowanie url z bazowego + endinga
    void handleError(QNetworkReply *reply); // obsługa błędów sieciowych.
    QJsonArray parseJsonArrayReply(QNetworkReply *reply);
    QJsonObject parseJsonObjectReply(QNetworkReply *reply);
};
