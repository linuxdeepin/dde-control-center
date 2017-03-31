#ifndef LOCATIONPROVIDER_H
#define LOCATIONPROVIDER_H

#include <QList>

#include "types.h"

class LocationProvider {
public:
    virtual QString preferredWeatherService() const = 0;
    virtual void loadData() = 0;
    virtual QList<City> match(const QString &input) const = 0;
};

#endif // LOCATIONPROVIDER_H
