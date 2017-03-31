#include "deepinlocationprovider.h"

#include <QNetworkReply>
#include <QEventLoop>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

static const QString DeepinLocationServiceUrl = "http://w.api.deepin.com/v1/location/";

DeepinLocationProvider::DeepinLocationProvider(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this))
{

}

void DeepinLocationProvider::loadData()
{

}

QList<City> DeepinLocationProvider::match(const QString &input) const
{
    QList<City> ret;

    const QString url = DeepinLocationServiceUrl + input;
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(url));

    // make it synchronous.
    QEventLoop eventLoop;
    QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray arr = doc.array();
    for (const QJsonValue &value : arr) {
        QJsonObject obj = value.toObject();

        const QString id = obj.value("geonameId").toString();
        const QString country = obj.value("countryName").toString();
        const QString region = "";
        const QString name = obj.value("name").toString();
        const QString localizedName = obj.value("name").toString();
        const double latitude = obj.value("latitude").toString().toDouble();
        const double longitude = obj.value("longitude").toString().toDouble();

        ret << City{id, country, region, name, localizedName, latitude, longitude};
    }

    return ret;
}

QString DeepinLocationProvider::preferredWeatherService() const
{
    return "owm";
}
