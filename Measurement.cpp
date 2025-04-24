#include "Measurement.h"
#include <QDebug>
#include <algorithm>
#include <QJsonArray>
#include <cmath>

Measurement::Measurement(const QJsonObject &json) {
    if (!json.contains("key") || !json.contains("values")) {
        throw std::invalid_argument("Brak wymaganych pól w JSON pomiaru");
    }

    m_paramCode = json["key"].toString();

    if (!json["values"].isArray()) {
        throw std::invalid_argument("Pole wartości powinno być tablicą");
    }

    QJsonArray values = json["values"].toArray();
    Q_FOREACH (const QJsonValue &val, values) {
        if (!val.isObject()) {
            continue; //lub throw????? (dla ścisłej walidacji)
        }

        QJsonObject item = val.toObject();
        if (!item.contains("date") || !item.contains("value")) {
            continue; //pomijamy niekompletne wpisy
        }

        DataPoint point;
        point.timestamp = QDateTime::fromString(item["date"].toString(), "yyyy-MM-dd HH:mm:ss");

        if (item["value"].isNull()) {
            point.value = std::numeric_limits<double>::quiet_NaN();
        } else if (item["value"].isDouble()) {
            point.value = item["value"].toDouble();
        } else if (item["value"].isString()) {
            //obsługa wartości ciągów, jeśli to konieczne
            point.value = item["value"].toString().toDouble();
        } else {
            point.value = std::numeric_limits<double>::quiet_NaN();
        }

        m_data.append(point);
    }
}

QString Measurement::toString() const {
    QString result = QString("Parametr: %1\nMeasurements:\n").arg(m_paramCode);
    for (const DataPoint& point : m_data) {
        result += QString("- %1: %2\n")
        .arg(point.timestamp.toString("yyyy-MM-dd HH:mm"),
             std::isnan(point.value) ? "NULL" : QString::number(point.value));
    }
    return result;
}

//implementacje metod analizy danych
double Measurement::maxValue() const {
    auto it = std::max_element(m_data.begin(), m_data.end(),
                               [](const DataPoint& a, const DataPoint& b) {
                                   return a.value < b.value;
                               });
    return (it != m_data.end()) ? it->value : NAN;
}
