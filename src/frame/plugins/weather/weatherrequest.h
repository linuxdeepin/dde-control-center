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

#ifndef WEATHERREQUEST_H
#define WEATHERREQUEST_H

#include "weatheritem.h"

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMap>
#include <QThread>

#include "types.h"

class QTimer;
class WInterface;
class LoaderCity;

class QSettings;

class WeatherRequest : public QObject
{
    Q_OBJECT

public:
    explicit WeatherRequest(QObject *parent = 0);
    ~WeatherRequest();

    const City &city() const;
    QString localizedCityName() const;

    int count() const;
    WeatherItem dayAt(int index);

    void refreshData(bool force = false);
    void sendRefreshSignal();

    void setCity(const City& city);
    void searchCity(const QString &input);

    void setPreferredWeatherService(const QString &preferredWeatherService);

signals:
    void fetchLocalizedCityNameDone(const QString &name);
    void dataRefreshed(QList<WeatherItem> &items);

    void searchCityDone(QList<City> &cities);

private slots:

    void requestWeatherForecast(double latitude, double longitude);
    void requestGeoNameInfo(const QString &geonameId);
    void requestGeoNameID(double latitude, double longitude);

    void processWeatherServiceReply();
    void processGeoNameInfoReply();
    void processGeoNameIdReply();

    void processSearchCityReply();

private:
    QString randomGeoNameKey() const;

private:
    City m_city;
    QString m_preferredWeatherService;
    bool m_isFahrenheitDegree;
    LoaderCity* m_loader;
    QSettings *m_settings;

    QList<WeatherItem> m_items;
    QNetworkAccessManager *m_manager;

    QTime m_lastRefreshTimestamp;
    QTimer *m_retryTimer;
    uint m_retryCount;

    void saveCityInfo();
    void restoreCityInfo();

    void restoreTemperatureFormat();

    void saveExtraInfo();
    void restoreExtraInfo();
};

class LoaderCity : public QThread
{
    Q_OBJECT
public:
    explicit LoaderCity(QObject* parent = 0);

    City city() const;

signals:
    void done(const City &city);

protected:
    void run();

private:
    City m_city;
};

#endif // WEATHERREQUEST_H
