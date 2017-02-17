#include "weatherrequest.h"
#include "networkutil.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QProcess>

#include <QDomDocument>

static const QString WeatherServiceHost = "http://w.api.deepin.com";
static const QString GeoNameServiceHost = "http://api.geonames.org";
static const QString GeoNameKey=  "wangyaohua";

WeatherRequest::WeatherRequest(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<City>();

    m_loader = new LoaderCity(this);
    m_manager = new QNetworkAccessManager(this);

    connect(m_loader, &LoaderCity::done, this, &WeatherRequest::setCity);
    connect(m_manager, &QNetworkAccessManager::finished, this, [](QNetworkReply *reply) { reply->deleteLater(); });

    m_loader->start();
}

WeatherRequest::~WeatherRequest()
{

}

void WeatherRequest::setCity(const City &city)
{
    m_city = city;
    refreshData();
}

void WeatherRequest::processWeatherServiceReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    m_items.clear();

    QByteArray ba = reply->readAll();
    QJsonArray items = QJsonDocument::fromJson(ba).array();
    for (QJsonValue val : items) {
        QJsonObject obj = val.toObject();

        WeatherItem item;
        item.setName(obj["name"].toString().toLower());
        item.setDescription(obj["description"].toString());
        QDateTime dt; dt.setTime_t(obj["date"].toInt());
        item.setDate(dt.date());
        item.setTemperature(obj["temperatureMin"].toInt(), obj["temperatureMax"].toInt());

        m_items << item;
    }

    emit dataRefreshed(m_items);
}

void WeatherRequest::processGeoNameIdReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray ba = reply->readAll();

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

            QString geoNameInfoUrl = QString("%1/get?geonameId=%2&username=%3").arg(GeoNameServiceHost) \
                    .arg(geonameId).arg(GeoNameKey);
            reply = m_manager->get(QNetworkRequest(geoNameInfoUrl));
            connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processGeoNameInfoReply);
            break;
        }
        geoname = geoname.nextSiblingElement("geoname");
    }
}

void WeatherRequest::processGeoNameInfoReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray ba = reply->readAll();

    QDomDocument domDocument;
    QString errorMsg;
    if (!domDocument.setContent(ba, false, &errorMsg)) {
        qDebug() << ba;
        qWarning() << "read xml content error! " << errorMsg;
    }

    m_city.localizedName = m_city.name;
    QDomElement root = domDocument.documentElement();
    QDomElement alternateName = root.firstChildElement("alternateName");
    while (!alternateName.isNull()) {
        if (alternateName.hasAttribute("isPreferredName")) {
            m_city.localizedName = alternateName.text();
            break;
        }
        alternateName = alternateName.nextSiblingElement("alternateName");
    }

    emit fetchLocalizedCityNameDone(m_city.localizedName);
}

QString WeatherRequest::city() const
{
    return m_city.name;
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

void WeatherRequest::refreshData()
{
    const int elapsed = m_lastRefreshTimestamp.elapsed();
    if ((elapsed >= 1000 * 60 * 15 || elapsed == 0) && !m_city.name.isEmpty()) {
        m_lastRefreshTimestamp.restart();

        City city = m_city;
        QString weatherUrl = QString("%1/forecast/%2/%3/%4").arg(WeatherServiceHost).arg(city.country) \
                        .arg(city.region).arg(city.name);
        QNetworkReply *reply = m_manager->get(QNetworkRequest(weatherUrl));
        connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processWeatherServiceReply);

        QString geoNameIDUrl = QString("%1/extendedFindNearby?lat=%2&lng=%3&username=%4").arg(GeoNameServiceHost) \
                .arg(city.latitude).arg(city.longitude).arg(GeoNameKey);
        reply = m_manager->get(QNetworkRequest(geoNameIDUrl));
        connect(reply, &QNetworkReply::finished, this, &WeatherRequest::processGeoNameIdReply);
    }
}

void WeatherRequest::sendRefreshSignal()
{
    emit dataRefreshed(m_items);
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
