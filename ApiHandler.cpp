#include "ApiHandler.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QSslConfiguration>

ApiHandler::ApiHandler(QObject *parent) : QObject(parent),
    m_apiBaseUrl("https://api.gios.gov.pl/pjp-api/rest"),
    m_isBusy(false)
{
    //weryfikacja obsługi SSL(opcjonalnie)
    if (!QSslSocket::supportsSsl()) {
        qCritical() << "SSL nie jest obsługiwany!";
        qDebug() << "Wersja kompilacji biblioteki SSL:" << QSslSocket::sslLibraryBuildVersionString();
    }
}

//podstawowe metody API
void ApiHandler::fetchStations() {
    if (m_isBusy) return;
    m_isBusy = true;

    QUrl url = buildUrl("station/findAll");
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &ApiHandler::handleStationsReply);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiHandler::handleNetworkError);
}

void ApiHandler::fetchSensors(int stationId) {
    QUrl url = buildUrl(QString("station/sensors/%1").arg(stationId));
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &ApiHandler::handleSensorsReply);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiHandler::handleNetworkError);
}

void ApiHandler::fetchMeasurements(int sensorId) {
    QUrl url = buildUrl(QString("data/getData/%1").arg(sensorId));
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &ApiHandler::handleMeasurementsReply);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiHandler::handleNetworkError);
}

void ApiHandler::fetchAirQualityIndex(int stationId) {
    QUrl url = buildUrl(QString("aqindex/getIndex/%1").arg(stationId));
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &ApiHandler::handleAirQualityIndexReply);
    connect(reply, &QNetworkReply::errorOccurred, this, &ApiHandler::handleNetworkError);
}

//procedury obsługi odpowiedzi
void ApiHandler::handleStationsReply() {
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(qobject_cast<QNetworkReply*>(sender()));
    m_isBusy = false;

    if (!reply || reply->error() != QNetworkReply::NoError) {
        emit networkError(reply ? reply->errorString() : "Nieprawidłowy obiekt odpowiedzi");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit apiError(QString("Błąd analizy JSON: %1").arg(parseError.errorString()));
        return;
    }

    if (!doc.isArray()) {
        emit apiError("Nieprawidłowy format odpowiedzi: oczekiwana tablica JSON");
        return;
    }

    QVector<Station> stations;
    foreach (const QJsonValue &value, doc.array()) {
        if (value.isObject()) {
            stations.append(Station(value.toObject()));
        }
    }

    if (stations.isEmpty()) {
        qDebug() << "Otrzymano listę pustych stacji";
    }

    m_allStations = stations;
    emit stationsFetched(stations);
}

void ApiHandler::handleSensorsReply() {
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(qobject_cast<QNetworkReply*>(sender()));

    if (!reply || reply->error() != QNetworkReply::NoError) {
        emit networkError(reply ? reply->errorString() : "Nieprawidłowy obiekt odpowiedzi");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit apiError(QString("Błąd analizy JSON: %1").arg(parseError.errorString()));
        return;
    }

    if (!doc.isArray()) {
        emit apiError("Nieprawidłowy format odpowiedzi: oczekiwana tablica JSON");
        return;
    }

    QVector<Sensor> sensors;
    foreach (const QJsonValue &value, doc.array()) {
        if (value.isObject()) {
            try {
                sensors.append(Sensor(value.toObject()));
            } catch (const std::exception &e) {
                qWarning() << "Nie udało się utworzyć czujnika:" << e.what();
            }
        }
    }

    emit sensorsFetched(sensors);
}

void ApiHandler::handleMeasurementsReply() {
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(qobject_cast<QNetworkReply*>(sender()));

    if (!reply || reply->error() != QNetworkReply::NoError) {
        emit networkError(reply ? reply->errorString() : "Nieprawidłowy obiekt odpowiedzi");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit apiError(QString("Błąd analizy JSON: %1").arg(parseError.errorString()));
        return;
    }

    if (!doc.isObject()) {
        emit apiError("Nieprawidłowy format odpowiedzi: oczekiwany obiekt JSON");
        return;
    }

    try {
        Measurement measurement(doc.object());
        emit measurementsFetched(measurement);
    } catch (const std::exception& e) {
        emit apiError(QString("Nie udało się przeanalizować pomiarów: %1").arg(e.what()));
    }
}

void ApiHandler::handleAirQualityIndexReply() {
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(qobject_cast<QNetworkReply*>(sender()));

    if (!reply || reply->error() != QNetworkReply::NoError) {
        emit networkError(reply ? reply->errorString() : "Nieprawidłowy obiekt odpowiedzi");
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit apiError(QString("Błąd analizy JSON: %1").arg(parseError.errorString()));
        return;
    }

    if (!doc.isObject()) {
        emit apiError("Nieprawidłowy format odpowiedzi: oczekiwany obiekt JSON");
        return;
    }

    try {
        AirQualityIndex index(doc.object());
        emit airQualityIndexFetched(index);
    } catch (const std::exception& e) {
        emit apiError(QString("Nie udało się przeanalizować indeksu jakości powietrza: %1").arg(e.what()));
        qCritical() << "Nieudane dane JSON:" << doc.toJson(QJsonDocument::Indented);
    }
}

//metody filtracji
void ApiHandler::filterStationsByCity(const QString& city) {
    QVector<Station> filtered;
    std::copy_if(m_allStations.begin(), m_allStations.end(), std::back_inserter(filtered),
                 [&city](const Station& s) { return s.isInCity(city); });
    emit stationsFiltered(filtered);
}

void ApiHandler::findStationsInRadius(double lat, double lon, double radiusKm) {
    QVector<Station> result;
    std::copy_if(m_allStations.begin(), m_allStations.end(), std::back_inserter(result),
                 [lat, lon, radiusKm](const Station& s) { return s.distanceTo(lat, lon) <= radiusKm; });
    emit stationsFiltered(result);
}

//metody pomocnicze
QUrl ApiHandler::buildUrl(const QString &endpoint) const {
    return QUrl(m_apiBaseUrl + "/" + endpoint);
}

QNetworkRequest ApiHandler::createRequest(const QUrl &url) const {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");

    QSslConfiguration sslConfig = request.sslConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater);
    request.setSslConfiguration(sslConfig);

    return request;
}

void ApiHandler::handleNetworkError(QNetworkReply::NetworkError code) {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QString errorString = reply ? reply->errorString() : "Nieznany błąd sieci";

    qWarning() << "Wystąpił błąd sieci:" << code << errorString;
    emit networkError(QString("Error %1: %2").arg(code).arg(errorString));

    if (reply) {
        reply->deleteLater();
    }
    m_isBusy = false;
}

void ApiHandler::setApiUrl(const QString &url) {
    if (m_apiBaseUrl != url) {
        m_apiBaseUrl = url;
        qDebug() << "URL bazy API zmieniono na:" << m_apiBaseUrl;
    }
}

void ApiHandler::handleGeocodingReply() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        qWarning() << "Otrzymano nieprawidłową odpowiedź na geokodowanie";
        return;
    }

    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> replyPtr(reply);

    if (reply->error() != QNetworkReply::NoError) {
        emit geocodingError(reply->errorString());
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit geocodingError("Błąd analizy JSON: " + parseError.errorString());
        return;
    }

    try {
        QJsonObject obj = doc.object();
        QJsonArray results = obj["results"].toArray();

        if (!results.isEmpty()) {
            QJsonObject firstResult = results.first().toObject();
            double lat = firstResult["geometry"].toObject()["location"].toObject()["lat"].toDouble();
            double lng = firstResult["geometry"].toObject()["location"].toObject()["lng"].toDouble();

            emit geocodingFinished(lat, lng);
        } else {
            emit geocodingError("Nie znaleziono wyników");
        }
    } catch (...) {
        emit geocodingError("Nie udało się przeanalizować odpowiedzi geokodowania");
    }
}

void ApiHandler::findStationsByAddress(const QString& address, double radiusKm) {
    QString encodedAddress = QUrl::toPercentEncoding(address);
    QUrl url(QString("https://maps.googleapis.com/maps/api/geocode/json?address=%1&key=YOUR_API_KEY")
                 .arg(encodedAddress));

    QNetworkRequest request(url);
    QNetworkReply* reply = m_geocoderManager.get(request);

    connect(reply, &QNetworkReply::finished,
            this, &ApiHandler::handleGeocodingReply);
    connect(reply, &QNetworkReply::errorOccurred,
            this, [this](QNetworkReply::NetworkError error) {
                emit geocodingError("Błąd sieci: " + QString::number(error));
            });
}

