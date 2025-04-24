#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_apiHandler(new ApiHandler(this)),
    m_allStations()
{
    ui->setupUi(this);

    //podlaczenie przycisków
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::handleRefreshClicked);
    connect(ui->filterButton, &QPushButton::clicked, this, &MainWindow::handleFilterClicked);
    connect(ui->mapButton, &QPushButton::clicked, this, &MainWindow::handleMapClicked);

    //podlaczenie list
    connect(ui->stationList, &QListWidget::itemClicked, this, &MainWindow::handleStationClicked);
    connect(ui->sensorList, &QListWidget::itemClicked, this, &MainWindow::handleSensorClicked);

    //podlaczenie sygnalow ApiHandler
    connect(m_apiHandler, &ApiHandler::stationsFetched, this, &MainWindow::handleStationsFetched);
    connect(m_apiHandler, &ApiHandler::sensorsFetched, this, &MainWindow::handleSensorsFetched);
    connect(m_apiHandler, &ApiHandler::measurementsFetched, this, &MainWindow::handleMeasurementsFetched);
    connect(m_apiHandler, &ApiHandler::airQualityIndexFetched, this, &MainWindow::handleAirQualityFetched);
    connect(m_apiHandler, &ApiHandler::networkError, this, &MainWindow::handleNetworkError);
    connect(m_apiHandler, &ApiHandler::stationsFiltered, this, &MainWindow::displayStations);

    //konfiguracja UI
    setWindowTitle("Aplikacja testowa");
    ui->statusbar->showMessage("System ready", 3000);
    m_apiHandler->fetchStations();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//gniazda na przyciski
void MainWindow::handleRefreshClicked()
{
    m_apiHandler->fetchStations();
    ui->statusbar->showMessage("Odświeżanie danych...", 2000);
}

void MainWindow::handleFilterClicked()
{
    QString city = ui->cityFilterEdit->text().trimmed();
    if (city.isEmpty()) {
        displayStations(m_allStations);
        return;
    }

    QVector<Station> filtered;
    std::copy_if(m_allStations.begin(), m_allStations.end(), std::back_inserter(filtered),
                 [city](const Station& s) {
                     return s.cityName().contains(city, Qt::CaseInsensitive);
                 });

    displayStations(filtered);
    logMessage(QString("Zastosowano filtr według miasta: %1").arg(city));
}

void MainWindow::handleMapClicked()
{
    logMessage("Mapa (pozniej)");
}

void MainWindow::handleStationClicked(QListWidgetItem *item)
{
    int stationId = item->data(Qt::UserRole).toInt();
    m_apiHandler->fetchSensors(stationId);
    m_apiHandler->fetchAirQualityIndex(stationId);
    logMessage(QString("Wybrana stacja ID: %1").arg(stationId));
}

void MainWindow::handleSensorClicked(QListWidgetItem *item)
{
    int sensorId = item->data(Qt::UserRole).toInt();
    m_apiHandler->fetchMeasurements(sensorId);
    logMessage(QString("Wybrano czujnik ID: %1").arg(sensorId));
}

//procesory danych z API
void MainWindow::handleStationsFetched(const QVector<Station>& stations)
{
    qDebug() << "Otrzymane stacje:" << stations.size();  // Вывод в консоль
    if (stations.isEmpty()) {
        qDebug() << "Lista stacji jest pusta! Sprawdź API.";
    }
    m_allStations = stations;
    displayStations(stations);
}

void MainWindow::handleSensorsFetched(const QVector<Sensor>& sensors)
{
    displaySensors(sensors);
    logMessage(QString("Otrzymane czujniki: %1").arg(sensors.size()));
}

void MainWindow::handleMeasurementsFetched(const Measurement& measurement)
{
    displayMeasurement(measurement);
    logMessage("Uzyskano nowe pomiary");
}

void MainWindow::handleAirQualityFetched(const AirQualityIndex& index)
{
    displayAirQuality(index);
    logMessage("Uzyskano wskaźnik jakości powietrza");
}

void MainWindow::handleNetworkError(const QString& message)
{
    QMessageBox::warning(this, "Błąd sieci", message);
    logMessage("Błąd: " + message);
}

//metody pomocnicze
void MainWindow::displayStations(const QVector<Station>& stations)
{
    ui->stationList->clear();  //czyszczenie listy
    for (const auto& station : stations) {
        QListWidgetItem *item = new QListWidgetItem(station.toShortString());
        item->setData(Qt::UserRole, station.id());  //zapis identyfikatora stacji
        ui->stationList->addItem(item);  //dodawanie do listy
    }
    qDebug() << "Dodane stacje do UI:" << ui->stationList->count();
}

void MainWindow::displaySensors(const QVector<Sensor>& sensors)
{
    ui->sensorList->clear();
    for (const auto& sensor : sensors) {
        QListWidgetItem *item = new QListWidgetItem(sensor.toString());
        item->setData(Qt::UserRole, sensor.id());
        ui->sensorList->addItem(item);
    }
}

void MainWindow::displayMeasurement(const Measurement& measurement)
{
    ui->measurementBrowser->clear();
    ui->measurementBrowser->setText(measurement.toString());
}

void MainWindow::displayAirQuality(const AirQualityIndex& index)
{
    ui->airQualityBrowser->clear();
    ui->airQualityBrowser->setText(index.toString());

    QPalette palette = ui->airQualityBrowser->palette();
    palette.setColor(QPalette::Base, index.getQualityColor());
    palette.setColor(QPalette::Text,
                     index.getQualityColor().lightness() < 128 ? Qt::white : Qt::black);
    ui->airQualityBrowser->setPalette(palette);
}

void MainWindow::logMessage(const QString& message)
{
    if (!ui->logBrowser) {
        qWarning() << "Przeglądarka dziennika nie została zainicjowana!";
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logBrowser->append(QString("[%1] %2").arg(timestamp, message));
    ui->statusbar->showMessage(message, 3000);
}
