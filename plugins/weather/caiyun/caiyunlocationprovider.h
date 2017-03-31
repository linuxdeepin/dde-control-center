#ifndef CAIYUNLOCATIONPROVIDER_H
#define CAIYUNLOCATIONPROVIDER_H

#include <QObject>
#include <QThread>

#include "locationprovider.h"

class CaiyunLocationProvider;
class LoadDataThread : public QThread
{
    Q_OBJECT
public:
    explicit LoadDataThread(CaiyunLocationProvider *parent);
    virtual ~LoadDataThread() {}

    void run() Q_DECL_OVERRIDE;

private:
    void loadData();
};

class CaiyunLocationProvider : public QObject, public LocationProvider
{
    Q_OBJECT
public:
    explicit CaiyunLocationProvider(QObject *parent = 0);

    void loadData() Q_DECL_OVERRIDE;
    QList<City> match(const QString &input) const Q_DECL_OVERRIDE;
    QString preferredWeatherService() const Q_DECL_OVERRIDE;

private:
    friend class LoadDataThread;

    QList<City> m_cities;
    LoadDataThread *m_loadDataThread;
};

#endif // CAIYUNLOCATIONPROVIDER_H
