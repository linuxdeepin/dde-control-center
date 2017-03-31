#ifndef DEEPINLOCATIONPROVIDER_H
#define DEEPINLOCATIONPROVIDER_H

#include <QObject>
#include <QNetworkAccessManager>

#include "locationprovider.h"

class DeepinLocationProvider : public QObject, public LocationProvider
{
    Q_OBJECT
public:
    explicit DeepinLocationProvider(QObject *parent = 0);

    void loadData() Q_DECL_OVERRIDE;
    QList<City> match(const QString &input) const Q_DECL_OVERRIDE;
    QString preferredWeatherService() const Q_DECL_OVERRIDE;

private:
    QNetworkAccessManager *m_networkManager;
};

#endif // DEEPINLOCATIONPROVIDER_H
