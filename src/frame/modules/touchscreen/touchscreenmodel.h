// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TOUCHSCREENMODEL_H
#define TOUCHSCREENMODEL_H

#include "types/touchscreeninfolist_v2.h"
#include "types/touchscreenmap.h"

#include <QObject>

namespace dcc {

namespace display {

class Monitor;
class TouchscreenWorker;
class TouchscreenModel : public QObject
{
    Q_OBJECT

public:
    friend class TouchscreenWorker;

public:
    explicit TouchscreenModel(QObject *parent = 0);

    inline int displayMode() const { return m_mode; }
    inline const QList<Monitor *> monitorList() const { return m_monitors; }

    inline TouchscreenInfoList_V2 touchscreenList() const { return m_touchscreenList; }
    void setTouchscreenList(const TouchscreenInfoList_V2 &touchscreenList);

    inline TouchscreenMap touchMap() const { return m_touchMap; }
    void setTouchMap(const TouchscreenMap &touchMap);

Q_SIGNALS:
    void displayModeChanged(const int mode) const;
    void monitorListChanged() const;
    void touchscreenListChanged() const;
    void touchscreenMapChanged() const;

private Q_SLOTS:
    void setDisplayMode(const int mode);
    void monitorAdded(Monitor *mon);
    void monitorRemoved(Monitor *mon);

private:
    int m_mode;
    QList<Monitor *> m_monitors;
    TouchscreenInfoList_V2 m_touchscreenList;
    TouchscreenMap m_touchMap;
};

} // namespace display

} // namespace dcc

#endif // TOUCHSCREENMODEL_H
