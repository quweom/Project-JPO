#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "ApiHandler.h"
#include "Station.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void handleStationsFetched(const QVector<Station>& stations); //lista stancji
    void handleSensorsFetched(const QVector<Sensor>& sensors); //czujniki stancji
    void handleMeasurementsFetched(const Measurement& measurement); //dane czyjnika
    void handleAirQualityFetched(const AirQualityIndex& index); // indeks jakosci powietrza
    void handleNetworkError(const QString& message); // blad sieci lub api

    void handleStationClicked(QListWidgetItem *item); //nacisk na stancje
    void handleSensorClicked(QListWidgetItem *item); // nacisk na czyjnik
    void handleRefreshClicked(); // button odswiez
    void handleFilterClicked(); //button filtruj
    void handleMapClicked();// button pokaz na mapie

private:
    Ui::MainWindow *ui; //wskaznik na qt interfejs
    ApiHandler *m_apiHandler; // obiekt dla zapytania API
    QVector<Station> m_allStations; // cash

    void displayStations(const QVector<Station>& stations); // wyswietlenie  stancji
    void displaySensors(const QVector<Sensor>& sensors); // wyswietlenie czujnikow
    void displayMeasurement(const Measurement& measurement);// wyswietlenie pomiarow
    void displayAirQuality(const AirQualityIndex& index);// wyswietlenie indeksu jakosci
    void logMessage(const QString& message);// co sie dzieje podczas pracy aplikacji
};

#endif // MAINWINDOW_H
