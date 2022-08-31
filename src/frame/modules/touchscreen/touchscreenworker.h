// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TOUCHSCREENWORKER_H
#define TOUCHSCREENWORKER_H

#include <com_deepin_daemon_display.h>

#include <QObject>

using DisplayInter = com::deepin::daemon::Display;

namespace dcc {

namespace display {

class Monitor;
class TouchscreenModel;
class TouchscreenWorker : public QObject
{
    Q_OBJECT

public:
    explicit TouchscreenWorker(TouchscreenModel *model, QObject *parent = 0, bool isSync = false);
    ~TouchscreenWorker();

    void active();

public Q_SLOTS:
    void setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID);

private Q_SLOTS:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);

private:
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);

private:
    TouchscreenModel *m_model;
    DisplayInter m_displayInter;
    QList<Monitor *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // TOUCHSCREENWORKER_H
