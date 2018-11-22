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

#include "miracastworker.h"
#include "miracastmodel.h"

using namespace dcc;
using namespace dcc::display;

MiracastWorker::MiracastWorker(MiracastModel *model, QObject *parent)
    : QObject(parent),
      m_miracastModel(model),
      m_miracastInter(new MiracastInter("com.deepin.daemon.Miracast", "/com/deepin/daemon/Miracast", QDBusConnection::sessionBus(), this)),
      m_timerPowerOff(new QTimer(this))
{
    connect(m_miracastInter, &MiracastInter::Added, m_miracastModel, &MiracastModel::onPathAdded);
    connect(m_miracastInter, &MiracastInter::Removed, m_miracastModel, &MiracastModel::onPathRemoved);
    connect(m_miracastInter, &MiracastInter::Event, m_miracastModel, &MiracastModel::onMiracastEvent);
    connect(m_miracastModel, &MiracastModel::requestLinkScanning, this, &MiracastWorker::setLinkScannning);
    connect(m_miracastModel, &MiracastModel::sinkConnected, m_timerPowerOff, &QTimer::stop);

    m_miracastInter->setSync(false);
    //set a timeout of 5 minutes
    m_timerPowerOff->setInterval(5 * 60 * 1000);
    m_timerPowerOff->setSingleShot(true);
    connect(m_timerPowerOff, &QTimer::timeout, this, &MiracastWorker::onPowerOff);
}

void MiracastWorker::active()
{
    m_miracastInter->blockSignals(false);
    queryLinks();
}

void MiracastWorker::deactive()
{
    m_miracastInter->blockSignals(true);
}

void MiracastWorker::fetchData()
{
    queryLinks();
}

void MiracastWorker::queryLinks()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_miracastInter->ListLinks());

    connect(w, &QDBusPendingCallWatcher::finished, this, &MiracastWorker::queryLinks_CB);
}

void MiracastWorker::querySinks()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_miracastInter->ListSinks());

    connect(w, &QDBusPendingCallWatcher::finished, this, &MiracastWorker::querySinks_CB);
}

void MiracastWorker::disconnectSink(const QDBusObjectPath &sink)
{
    qDebug() << Q_FUNC_INFO << sink.path();

    m_miracastInter->Disconnect(sink);

    m_timerPowerOff->start();
}

void MiracastWorker::connectSink(const QDBusObjectPath &peer, const QRect area)
{
    qDebug() << Q_FUNC_INFO << peer.path() << area;

    m_miracastInter->Connect(peer, area.x(), area.y(), area.width(), area.height());
}

void MiracastWorker::setLinkEnable(const QDBusObjectPath &path, const bool enable)
{
    qDebug() << Q_FUNC_INFO << path.path() << enable;

    m_miracastInter->Enable(path, enable);
}

void MiracastWorker::setLinkScannning(const QDBusObjectPath &path, const bool scanning)
{
    qDebug() << Q_FUNC_INFO << path.path() << scanning;

    m_miracastInter->Scanning(path, scanning);

    m_timerPowerOff->start();
}

void MiracastWorker::queryLinks_CB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<LinkInfoList> reply = *w;

    m_miracastModel->setLinks(reply.value());

    w->deleteLater();

    // query sinks when we get all links.
    querySinks();
}

void MiracastWorker::querySinks_CB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<SinkInfoList> reply = *w;

    m_miracastModel->setSinks(reply.value());

    w->deleteLater();
}

void MiracastWorker::onPowerOff()
{
    // shutdown all link
    for (LinkInfo link : m_miracastModel->links())
        m_miracastInter->Enable(link.m_dbusPath, false);
}
