#ifndef LOCATIONPROVIDER_H
#define LOCATIONPROVIDER_H

#include <QList>

#include "types.h"

class LocationProvider {
public:
    virtual void loadData() = 0;
    virtual QList<City> match(const QString &input) const = 0;

protected:
    QList<City> m_cities;
};

#endif // LOCATIONPROVIDER_H
