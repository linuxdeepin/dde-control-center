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

#include "weatheritem.h"

WeatherItem::WeatherItem(QObject *parent) :
    QObject(parent)
{

}

WeatherItem::WeatherItem(const WeatherItem &item) :
    QObject(),
    m_name(item.name()),
    m_date(item.date()),
    m_temperature(item.temperature()),
    m_fahrenheit(item.m_fahrenheit)
{

}

WeatherItem::~WeatherItem()
{

}

QString WeatherItem::name() const
{
    return m_name;
}

void WeatherItem::setName(const QString &name)
{
    m_name = name;
}

void WeatherItem::setFahrenheitDegree(const bool fahrenheit)
{
    m_fahrenheit = fahrenheit;
}

bool WeatherItem::isFahrenheit() const
{
    return m_fahrenheit;
}

QPair<int, int> WeatherItem::temperature() const
{
    return m_temperature;
}

void WeatherItem::setTemperature(const int &min, const int &max)
{
    setTemperature(QPair<int,int>(min, max));
}

void WeatherItem::setTemperature(const QPair<int, int> &temperature)
{
    m_temperature = temperature;
}

QDate WeatherItem::date() const
{
    return m_date;
}

void WeatherItem::setDate(const QDate &date)
{
    m_date = date;
}

QString WeatherItem::dayName() const
{
    return m_date.toString("ddd");
}

WeatherItem &WeatherItem::operator=(const WeatherItem &rhs)
{
    if (this != &rhs) {
        m_name = rhs.name();
        m_date = rhs.date();
        m_description = rhs.description();
        m_temperature = rhs.temperature();
        m_fahrenheit  = rhs.isFahrenheit();
    }

    return *this;
}

QString WeatherItem::description() const
{
    return m_description;
}

void WeatherItem::setDescription(const QString &description)
{
    m_description = description;
}

QDebug operator <<(QDebug debug, const WeatherItem &item)
{
    return debug << QString("name:%1,description:%2,date:%3,temperature:{%4, %5},fahrenheit:%6")
                    .arg(item.m_name)
                    .arg(item.m_description)
                    .arg(item.m_date.toString())
                    .arg(item.m_temperature.first)
                    .arg(item.m_temperature.second)
                    .arg(item.m_fahrenheit);
}
