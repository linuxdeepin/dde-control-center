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

    void refreshData();
    void sendRefreshSignal();

signals:
    void fetchLocalizedCityNameDone(const QString &name);
    void dataRefreshed(QList<WeatherItem> &items);

private slots:
    void replyFinished(QNetworkReply* reply);
    void slotTimeout();
    void setCity(const City& city);

    void processWeatherServiceReply();
    void processGeoNameIdReply();
    void processGeoNameInfoReply();

private:
    City m_city;
    LoaderCity* m_loader;

    QList<WeatherItem> m_items;
    QNetworkAccessManager *m_manager;

    QTime m_lastRefreshTimestamp;
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
