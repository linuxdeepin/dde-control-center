#include "weatherrequest.h"
#include "networkutil.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QProcess>

WeatherRequest::WeatherRequest(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<City>();

    m_loader = new LoaderCity(this);
    m_manager = new QNetworkAccessManager(this);

    connect(m_loader, &LoaderCity::done, this, &WeatherRequest::setCity);
    connect(m_manager, &QNetworkAccessManager::finished, this, &WeatherRequest::replyFinished);

    m_loader->start();
}

WeatherRequest::~WeatherRequest()
{

}

void WeatherRequest::setCity(const City &city)
{
    m_city = city;

    QString url = QString("http://hualet.org:9898/forecast/%1/%2/%3").arg(city.country) \
                    .arg(city.region).arg(city.city);
    qDebug() << url;
    m_manager->get(QNetworkRequest(url));
}

QString WeatherRequest::city() const
{
    return m_city.city;
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

void WeatherRequest::sendRefreshSignal()
{
    emit refreshData(m_items);
}

void WeatherRequest::replyFinished(QNetworkReply *reply)
{
    m_items.clear();

    QByteArray ba = reply->readAll();
    reply->deleteLater();

    QJsonArray items = QJsonDocument::fromJson(ba).array();
    for (QJsonValue val : items) {
        QJsonObject obj = val.toObject();

        WeatherItem item;
        item.setName(obj["name"].toString().toLower());
        item.setDescription(obj["description"].toString());
        item.setDate(QDate::fromString(obj["date"].toString(), "yyyy-MM-dd"));
        item.setTemperature(obj["temperatureMin"].toInt(), obj["temperatureMax"].toInt());

        m_items << item;
    }

    emit refreshData(m_items);
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
