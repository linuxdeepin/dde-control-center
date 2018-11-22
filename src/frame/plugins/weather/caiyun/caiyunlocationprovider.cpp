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
