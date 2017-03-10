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

    QString city() const;
    QString localizedCityName() const;

    int count() const;
    WeatherItem dayAt(int index);

    void refreshData(bool force = false);
    void sendRefreshSignal();

    void setCity(const City& city);
    void searchCity(const QString &input);

signals:
    void fetchLocalizedCityNameDone(const QString &name);
    void dataRefreshed(QList<WeatherItem> &items);

    void searchCityDone(QList<City> &cities);

private slots:
    void replyFinished(QNetworkReply* reply);
    void slotTimeout();

    void requestWeatherForecast(const QString &geonameId);
    void requestGeoNameInfo(const QString &geonameId);

    void processWeatherServiceReply();
    void processGeoNameIdReply();
    void processGeoNameInfoReply();

    void processSearchCityReply();

private:
    City m_city;
    LoaderCity* m_loader;
    QSettings *m_settings;

    QList<WeatherItem> m_items;
    QNetworkAccessManager *m_manager;

    QTime m_lastRefreshTimestamp;
    QTimer *m_retryTimer;

    void saveGeoNameID(const QString &geonameId);
    QString restoreGeoNameID() const;
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
