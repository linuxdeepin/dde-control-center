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
