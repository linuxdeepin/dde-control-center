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

//    void setCity(const QString& city);
    QString city() const;
    int count() const;
    WeatherItem dayAt(int index);

    void sendRefreshSignal();

signals:
    void refreshData(QList<WeatherItem> &items);

public slots:
    void replyFinished(QNetworkReply* reply);
    void slotTimeout();
    void setCity(const QString& city);

private:
    QUrl m_url;
    QUrl m_location;
    QTimer *m_timer;
    QString m_city;
    LoaderCity* m_loader;
    QList<WeatherItem> m_items;
    QNetworkAccessManager *m_manager;
    QMap<QString, WInterface*> m_maps;
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
