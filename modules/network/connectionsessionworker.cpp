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

#include "connectionsessionworker.h"
#include "connectionsessionmodel.h"

using namespace dcc::network;

ConnectionSessionWorker::ConnectionSessionWorker(const QString &sessionPath, ConnectionSessionModel *model, QObject *parent)
    : QObject(parent),

      m_connModel(model),
      m_sessionInter("com.deepin.daemon.Network", sessionPath, QDBusConnection::sessionBus(), this)
{
    m_sessionInter.setSync(false);

    connect(&m_sessionInter, &ConnectionSessionInter::AvailableKeysChanged, this, &ConnectionSessionWorker::queryAvailableKeys);
    connect(&m_sessionInter, &ConnectionSessionInter::ErrorsChanged, m_connModel, &ConnectionSessionModel::setErrors);
    connect(&m_sessionInter, &ConnectionSessionInter::UuidChanged, m_connModel, &ConnectionSessionModel::setConnUuid);
    connect(&m_sessionInter, &ConnectionSessionInter::TypeChanged, m_connModel, &ConnectionSessionModel::setType);
    connect(&m_sessionInter, &ConnectionSessionInter::AllowDeleteChanged, m_connModel, &ConnectionSessionModel::setDeletable);

    m_connModel->setErrors(m_sessionInter.errors());
    m_connModel->setConnUuid(m_sessionInter.uuid());
    m_connModel->setType(m_sessionInter.type());
    m_connModel->setDeletable(m_sessionInter.allowDelete());

    queryAllKeys();
    queryAvailableKeys();
}

void ConnectionSessionWorker::closeSession()
{
    if (m_sessionInter.isValid())
        m_sessionInter.Close();
}

void ConnectionSessionWorker::queryAllKeys()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_sessionInter.GetAllKeys(), this);

    connect(w, &QDBusPendingCallWatcher::finished, this, &ConnectionSessionWorker::queryAllKeysCB);
}

void ConnectionSessionWorker::saveSettings(const bool active)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_sessionInter.Save(active), this);

    connect(w, &QDBusPendingCallWatcher::finished, this, &ConnectionSessionWorker::saveSettingsCB);
}

void ConnectionSessionWorker::changeSettings(const QString &section, const QString &vKey, const QString &data)
{
#ifdef QT_DEBUG
    qDebug() << section << vKey << data;
#endif

    m_sessionInter.SetKey(section, vKey, data);
}

void ConnectionSessionWorker::queryAvailableKeys()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_sessionInter.ListAvailableKeyDetail(), this);

    connect(w, &QDBusPendingCallWatcher::finished, this, &ConnectionSessionWorker::queryAvailableKeysCB);
}

void ConnectionSessionWorker::queryAllKeysCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    m_connModel->setAllKeys(reply.value());

    w->deleteLater();
}

void ConnectionSessionWorker::queryAvailableKeysCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QString> reply = *w;

    m_connModel->setAvailableItems(reply.value());

    w->deleteLater();
}

void ConnectionSessionWorker::saveSettingsCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<bool> reply = *w;

    emit m_connModel->saveFinished(reply.value());

    w->deleteLater();
}
