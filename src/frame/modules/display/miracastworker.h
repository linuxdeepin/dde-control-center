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

#ifndef MIRACASTWORKER_H
#define MIRACASTWORKER_H

#include <QObject>
#include <QTimer>
#include <com_deepin_daemon_miracast.h>

using MiracastInter = com::deepin::daemon::Miracast;

namespace dcc {
namespace display {
class MiracastModel;
class MiracastWorker : public QObject
{
    Q_OBJECT

public:
    explicit MiracastWorker(MiracastModel *model, QObject *parent = 0);
    void active();
    void deactive();
    void fetchData();
    void queryLinks();
    void querySinks();

public Q_SLOTS:
    void disconnectSink(const QDBusObjectPath &sink);
    void connectSink(const QDBusObjectPath &peer, const QRect area);
    void setLinkEnable(const QDBusObjectPath &path, const bool enable);
    void setLinkScannning(const QDBusObjectPath &path, const bool scanning);

private Q_SLOTS:
    void queryLinks_CB(QDBusPendingCallWatcher *w);
    void querySinks_CB(QDBusPendingCallWatcher *w);
    void onPowerOff();

private:
    MiracastModel *m_miracastModel;
    MiracastInter *m_miracastInter;
    QTimer *m_timerPowerOff;
};
}
}

#endif // MIRACASTWORKER_H
