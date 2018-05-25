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

#ifndef WEATHERITEM_H
#define WEATHERITEM_H

#include <QObject>
#include <QDate>
#include <QDebug>

class WeatherItem : public QObject
{
    Q_OBJECT
public:
    explicit WeatherItem(QObject *parent = 0);
    WeatherItem(const WeatherItem &item);
    ~WeatherItem();

    QString name() const;
    void setName(const QString &name);

    void setFahrenheitDegree(const bool fahrenheit);
    bool isFahrenheit() const;

    QPair<int, int> temperature() const;
    void setTemperature(const int &min, const int &max);
    void setTemperature(const QPair<int, int> &temperature);

    QDate date() const;
    void setDate(const QDate &date);

    QString dayName() const;

    WeatherItem& operator=(const WeatherItem &rhs);

    QString description() const;
    void setDescription(const QString &description);

    friend QDebug operator <<(QDebug debug, const WeatherItem &item);

private:
    QString m_name;
    QString m_description;
    QDate m_date;
    QPair<int,int> m_temperature;
    bool m_fahrenheit;
};



#endif // WEATHERITEM_H
