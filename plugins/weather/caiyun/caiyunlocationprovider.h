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
