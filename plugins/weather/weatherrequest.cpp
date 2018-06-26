/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "weatherrequest.h"
#include "networkutil.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QProcess>
#include <QLocale>
#include <QSettings>

#include <QDomDocument>

static const QString WeatherServiceHost = "http://w.api.deepin.com/v1";
static const QString GeoNameServiceHost = "http://api.geonames.org";
static const QStringList GeoNameKeys =  {"wangyaohua", "change", "position", "apple", "free"};

static const QString GroupLocation = "Location";
static const QString KeyLatitude = "Latitude";
static const QString KeyLongitude = "Longitude";
static const QString KeyLocalizedName = "LocalizedName";
static const QString KeyPreferredService = "PreferredService";
static const QString KeyTemperatureFormat = "TemperatureFormat";

static void errorCheck(const QString &funcInfo, const QNetworkReply::NetworkError &error) {
    if (error != QNetworkReply::NoError) {
        qDebug() << funcInfo << error;
    }
}

WeatherRequest::WeatherRequest(QObject *parent) :
    QObject(parent),
    m_retryTimer(new QTimer(this)),
    m_retryCount(0)
{
    qRegisterMetaType<City>();

    m_settings = new QSettings("deepin", "dcc-weather-plugin");
    restoreCityInfo();
    restoreExtraInfo();
    restoreTemperatureFormat();

    m_loader = new LoaderCity(this);
    m_manager = new QNetworkAccessManager(this);

    connect(m_loader, &LoaderCity::done, this, &WeatherRequest::setCity);
    connect(m_manager, &QNetworkAccessManager::finished, this, [](QNetworkReply *reply) { reply->deleteLater(); });
    connect(m_manager, &QNetworkAccessManager::networkAccessibleChanged, this, [this] { m_retryCount = 0; });

    m_retryTimer->setSingleShot(false);
    m_retryTimer->setInterval(10 * 60 * 1000);

    auto func = [this] {
        if (m_retryCount >= 10) return;

        qDebug() << "retry timer timeout";
        m_retryCount++;
        if (m_city.latitude == 0 || m_city.longitude == 0) {
            m_loader->start();
        } else {
            refreshData(true);
        }
    };
    connect(m_retryTimer, &QTimer::timeout, this, func);


    QTimer::singleShot(0, func);
    m_retryTimer->start();
}

WeatherRequest::~WeatherRequest()
{

}

const City &WeatherRequest::city() const
{
    return m_city;
}

void WeatherRequest::setCity(const City &city)
{
    m_city = city;
    saveCityInfo();
    refreshData(true);
}

void WeatherRequest::processWeatherServiceReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    errorCheck(Q_FUNC_INFO, reply->error());

    m_items.clear();

    QByteArray ba = reply->readAll();
    qDebug() << "got weather info size: " << ba.size();
    QJsonArray items = QJsonDocument::fromJson(ba).array();
    for (QJsonValue val : items) {
        QJsonObject obj = val.toObject();

        WeatherItem item;
        item.setName(obj["name"].toString().toLower());
        item.setDescription(obj["description"].toString());
        item.setFahrenheitDegree(m_isFahrenheitDegree);
        QDateTime dt; dt.setTime_t(obj["date"].toInt());
        item.setDate(dt.date());
        item.setTemperature(obj["temperatureMin"].toInt(), obj["temperatureMax"].toInt());

        m_items << item;
    }

    if (m_items.length()) {
        emit dataRefreshed(m_items);
        m_retryTimer->stop();
        m_lastRefreshTimestamp.restart();
    }
}

void WeatherRequest::processGeoNameIdReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray ba = reply->readAll();

    errorCheck(Q_FUNC_INFO, reply->error());

    qDebug() << "got geo id size: " << ba;

    QDomDocument domDocument;
    QString errorMsg;
    if (!domDocument.setContent(ba, false, &errorMsg)) {
        qDebug() << ba;
        qWarning() << "read xml content error! " << errorMsg;
    }

    QDomElement root = domDocument.documentElement();
    QDomElement geoname = root.firstChildElement("geoname");
    while (!geoname.isNull()) {
        QString name = geoname.firstChildElement("name").text();
        if (name.toLower() == m_city.name.toLower()) {
            QString geonameId = geoname.firstChildElement("geonameId").text();
            qDebug() << "got geoname id " << geonameId;
            m_city.id = geonameId;
            requestGeoNameInfo(geonameId);
            break;
        }
        geoname = geoname.nextSiblingElement("geoname");
    }
}

void WeatherRequest::processGeoNameInfoReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray ba = reply->readAll();

    errorCheck(Q_FUNC_INFO, reply->error());

    QDomDocument domDocument;
    QString errorMsg;
    if (!domDocument.setContent(ba, false, &errorMsg)) {
        qDebug() << ba;
        qWarning() << "read xml content error! " << errorMsg;
    }

    if (!m_city.name.isEmpty())
        m_city.localizedName = m_city.name;

    QDomElement root = domDocument.documentElement();
    QDomElement name = root.firstChildElement("name");
    if (!name.text().isEmpty()) {
        m_city.localizedName = name.text();
        qDebug() << "got localized city name" << m_city.localizedName;
    }

    saveCityInfo();
    emit fetchLocalizedCityNameDone(m_city.localizedName);
}

void WeatherRequest::processSearchCityReply()
{
    QList<City> cities;

    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray ba = reply->readAll();

    errorCheck(Q_FUNC_INFO, reply->error());

    QDomDocument domDocument;
    QString errorMsg;
    if (!domDocument.setContent(ba, false, &errorMsg)) {
        qDebug() << ba;
        qWarning() << "read xml content error! " << errorMsg;
    }

    QDomElement root = domDocument.documentElement();
    QDomElement geoname = root.firstChildElement("geoname");
    while (!geoname.isNull()) {
        QString name = geoname.firstChildElement("name").text();
        QString geonameId = geoname.firstChildElement("geonameId").text();
        QString countryName = geoname.firstChildElement("countryName").text();
        double latitude = geoname.firstChildElement("lat").text().toDouble();
        double longitude = geoname.firstChildElement("lng").text().toDouble();

        cities.append(City{geonameId, countryName, "", name, name, latitude, longitude});

        geoname = geoname.nextSiblingElement("geoname");
    }

    emit searchCityDone(cities);
}

QString WeatherRequest::randomGeoNameKey() const
{
    const QString key = GeoNameKeys.at(qrand() % GeoNameKeys.length());
    qDebug() << "using random geoname key" << key;
    return key;
}

void WeatherRequest::setPreferredWeatherService(const QString &preferredWeatherService)
{
    m_preferredWeatherService = preferredWeatherService;
    saveExtraInfo();
}

void WeatherRequest::saveCityInfo()
{
    m_settings->beginGroup(GroupLocation);
    m_settings->setValue(KeyLatitude, m_city.latitude);
    m_settings->setValue(KeyLongitude, m_city.longitude);
    m_settings->setValue(KeyLocalizedName, m_city.localizedName);
    m_settings->endGroup();
}

void WeatherRequest::restoreCityInfo()
{
    m_settings->beginGroup(GroupLocation);
    m_city.latitude = m_settings->value(KeyLatitude, "0").toDouble();
    m_city.longitude = m_settings->value(KeyLongitude, "0").toDouble();
    m_city.localizedName = m_settings->value(KeyLocalizedName, "").toString();
    m_settings->endGroup();
}

void WeatherRequest::restoreTemperatureFormat()
{
    m_settings->beginGroup(GroupLocation);
    m_isFahrenheitDegree = m_settings->value(KeyTemperatureFormat).toBool();
    m_settings->endGroup();
}

void WeatherRequest::saveExtraInfo()
{
    m_settings->beginGroup(GroupLocation);
    m_settings->setValue(KeyPreferredService, m_preferredWeatherService);
    m_settings->endGroup();
}

void WeatherRequest::restoreExtraInfo()
{
    m_settings->beginGroup(GroupLocation);
    m_preferredWeatherService = m_settings->value(KeyPreferredService).toString();
    m_settings->endGroup();
}

QString WeatherRequest::localizedCityName() const
{
    return m_city.localizedName;
}

int WeatherRequest::count() const
{
    return m_items.count();
}

WeatherItem WeatherRequest::dayAt(int index)
{
    if ( index >= 0 && index < m_items.count() ) {
        return m_items[index];
    }

    return WeatherItem();
}

void WeatherRequest::refreshData(bool force)
{
    const int elapsed = m_lastRefreshTimestamp.elapsed();
    if ((elapsed >= 1000 * 60 * 15 || force)) {
        qDebug() << "refreshing data";
        m_retryTimer->start();

        City city = m_city;

        if (city.latitude != 0 && city.longitude != 0) {
            requestWeatherForecast(city.latitude, city.longitude);
        }

        if (city.localizedName.isEmpty()) {
            requestGeoNameID(city.latitude, city.longitude);
        }
    }
}

void WeatherRequest::sendRefreshSignal()
{
    emit dataRefreshed(m_items);
}

void WeatherRequest::searchCity(const QString &input)
{
    qDebug() << "search city with input " << input;
    const QString lang = QLocale::system().name().split("_").at(0);
    QString searchCityUrl = QString("%1/search?q=%2&maxRows=10&username=%3&lang=%4").arg(GeoNameServiceHost) \
            .arg(input).arg(randomGeoNameKey()).arg(lang);
    QNetworkReply *reply = m_manager->get(QNetworkRequest(searchCityUrl));
    connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processSearchCityReply);
}

void WeatherRequest::requestWeatherForecast(double latitude, double longitude)
{
    qDebug() << "request weather forecast " << latitude << longitude;
    const QString service = m_preferredWeatherService.isEmpty() ? "" : m_preferredWeatherService + "/";
    QString weatherUrl = QString("%1/forecast/%2%3/%4").arg(WeatherServiceHost).arg(service) \
            .arg(latitude).arg(longitude);
    QNetworkReply *reply = m_manager->get(QNetworkRequest(weatherUrl));
    connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processWeatherServiceReply);
}

void WeatherRequest::requestGeoNameInfo(const QString &geonameId)
{
    if (!m_city.localizedName.isEmpty()) return;

    qDebug() << "request geoname city info " << geonameId;
    const QString lang = QLocale::system().name().split("_").at(0);
    QString geoNameInfoUrl = QString("%1/get?geonameId=%2&username=%3&lang=%4").arg(GeoNameServiceHost) \
            .arg(geonameId).arg(randomGeoNameKey()).arg(lang);
    QNetworkReply *reply = m_manager->get(QNetworkRequest(geoNameInfoUrl));
    connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processGeoNameInfoReply);
}

void WeatherRequest::requestGeoNameID(double latitude, double longitude)
{
    QString geoNameIDUrl = QString("%1/extendedFindNearby?lat=%2&lng=%3&username=%4").arg(GeoNameServiceHost) \
            .arg(latitude).arg(longitude).arg(randomGeoNameKey());
    QNetworkReply *reply = m_manager->get(QNetworkRequest(geoNameIDUrl));
    connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processGeoNameIdReply);
}

LoaderCity::LoaderCity(QObject *parent)
    :QThread(parent)
{

}

void LoaderCity::run()
{
    m_city = NetworkUtil::city();

    emit done(m_city);
}

City LoaderCity::city() const
{
    return m_city;
}
