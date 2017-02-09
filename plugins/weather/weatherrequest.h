#ifndef WEATHERREQUEST_H
#define WEATHERREQUEST_H

#include "weatheritem.h"

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMap>
#include <QThread>

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
    int count() const;
    WeatherItem dayAt(int index);

    void sendRefreshSignal();

signals:
    void refreshData(QList<WeatherItem> &items);

private slots:
    void replyFinished(QNetworkReply* reply);
    void slotTimeout();
    void setCity(const QString& city);

private:
    QString m_city;
    LoaderCity* m_loader;
    QList<WeatherItem> m_items;
    QNetworkAccessManager *m_manager;
};

class LoaderCity : public QThread
{
    Q_OBJECT
public:
    explicit LoaderCity(QObject* parent = 0);

signals:
    void city(const QString& city);

protected:
    void run();
};

#endif // WEATHERREQUEST_H
