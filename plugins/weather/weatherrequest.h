#ifndef WEATHERREQUEST_H
#define WEATHERREQUEST_H

#include "weatheritem.h"

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMap>

class QTimer;
class WInterface;

class WeatherRequest : public QObject
{
    Q_OBJECT

public:
    explicit WeatherRequest(QObject *parent = 0);
    ~WeatherRequest();

    void setCity(const QString& city);
    QString city() const;
    int count() const;
    WeatherItem dayAt(int index);


signals:
    void refreshData(QList<WeatherItem> &items);

public slots:
    void replyFinished(QNetworkReply* reply);
    void slotTimeout();

private:
    QUrl m_url;
    QUrl m_location;
    QTimer *m_timer;
    QString m_city;
    QList<WeatherItem> m_items;
    QNetworkAccessManager *m_manager;
    QMap<QString, WInterface*> m_maps;
};

#endif // WEATHERREQUEST_H
