#include "caiyunlocationprovider.h"

#include <QFile>
#include <QDebug>

CaiyunLocationProvider::CaiyunLocationProvider(QObject *parent)
    : QObject(parent),
      m_loadDataThread(new LoadDataThread(this))
{

}

void CaiyunLocationProvider::loadData()
{
    m_loadDataThread->start();
}

QList<City> CaiyunLocationProvider::match(const QString &input) const
{
    QList<City> data = m_cities;

    QList<City> ret;

    for (const City city : data) {
        if (city.name.contains(input) || city.region.contains(input) || city.country.contains(input)) {
            ret << city;
        }
    }

    return ret;
}

QString CaiyunLocationProvider::preferredWeatherService() const
{
    return "moji";
}

LoadDataThread::LoadDataThread(CaiyunLocationProvider *parent) :
    QThread(parent)
{

}

void LoadDataThread::run()
{
    loadData();
}

void LoadDataThread::loadData()
{
    CaiyunLocationProvider *provider = qobject_cast<CaiyunLocationProvider*>(parent());
    if (!provider) return;

    QFile file(":/weather/caiyun/cityidloc.csv");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = file.readLine();
        while (!content.isEmpty()) {
            QStringList tokens = content.split(",");

            City city;
            city.id = tokens.at(0);
            city.name = tokens.at(1);
            city.region = tokens.at(2);
            city.country = tokens.at(3);
            city.localizedName = city.name;
            city.latitude = tokens.at(4).toDouble();
            city.longitude = tokens.at(5).toDouble();

            provider->m_cities << city;

            // continue
            content = file.readLine();
        }
        file.close();
    }
}
