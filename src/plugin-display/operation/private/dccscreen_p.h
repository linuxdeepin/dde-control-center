// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCSCREEN_P_H
#define DCCSCREEN_P_H

#include "monitor.h"

#include <QObject>

namespace dccV25 {
const float MinScreenWidth = 1024.0f;
const float MinScreenHeight = 768.0f;

class DisplayWorker;
class DccScreen;
class DccScreenItem;

class DccScreenItemPrivate
{
public:
    static DccScreenItem *New(Monitor * monitor, DccScreen *screen);
    static DccScreenItemPrivate *Private(DccScreenItem *screenItem);

    explicit DccScreenItemPrivate(DccScreenItem *screenItem);
    virtual ~DccScreenItemPrivate();

    Monitor * m_monitor;
    DccScreenItem *q_ptr;
    Q_DECLARE_PUBLIC(DccScreenItem)
};

class DccScreenPrivate
{
public:
    static DccScreen *New(QList<Monitor *> monitors, DisplayWorker *worker, QObject *parent = nullptr);
    static DccScreenPrivate *Private(DccScreen *screen);

    explicit DccScreenPrivate(DccScreen *screen);
    virtual ~DccScreenPrivate();
    void setMonitors(QList<Monitor *> monitors);
    Monitor *monitor();
    QList<Monitor *> monitors();

    inline DisplayWorker *worker() { return m_worker; }

    void setMode(QSize resolution, double rate);
    void setRotate(uint rotate);
    void setFillMode(const QString &fileMode);
    void setScale(qreal scale);

    void updateResolutionList();
    void updateRateList();
    void updateScreen();
    void updateMaxScale();

private:
    DccScreen *q_ptr;
    QList<Monitor *> m_monitors;
    QList<QSize> m_resolutionList;
    QList<double> m_rateList;
    QString m_name;
    DisplayWorker *m_worker;
    QScreen *m_screen;
    qreal m_maxScale;
    QList<DccScreenItem *> m_screenItems;
    Q_DECLARE_PUBLIC(DccScreen)
};
} // namespace dccV25
#endif // DCCSCREEN_P_H
