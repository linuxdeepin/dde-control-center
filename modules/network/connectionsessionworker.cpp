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

void ConnectionSessionWorker::changeSettings(const QString &section, const QString &vKey, const QString &data, const bool encrypt)
{
#ifdef QT_DEBUG
    qDebug() << section << vKey << data << encrypt;
#endif

    if (!encrypt)
        m_sessionInter.SetKeyQueued(section, vKey, data);
    else
        changeSettingsEncrypted(section, vKey, data);
}

void ConnectionSessionWorker::changeSettingsEncrypted(const QString &section, const QString &vKey, const QString &data)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_sessionInter.SetKeyFd(section, vKey), this);
    w->setProperty("encrypted", data);

    connect(w, &QDBusPendingCallWatcher::finished, this, &ConnectionSessionWorker::changeSettingsEncryptedCB);
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

void ConnectionSessionWorker::changeSettingsEncryptedCB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QDBusUnixFileDescriptor> reply = *w;
    w->deleteLater();

    QFile f;
    if (!f.open(reply.value().fileDescriptor(), QIODevice::WriteOnly))
    {
        qWarning() << Q_FUNC_INFO << "write encrypted data failed, open fd error." << f.errorString();
        return;
    }

    const QString &encryptedData = w->property("encrypted").toString();
    const quint32 length = encryptedData.size();

    // TODO: only support big/little endians.
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    const char *lengthBytes = reinterpret_cast<const char *>(&length);
#else
    Q_ASSERT(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__);

    const char lengthBytes[] = { char(length >> 0  & 0xff),
                                 char(length >> 8  & 0xff),
                                 char(length >> 16 & 0xff),
                                 char(length >> 24 & 0xff), };
#endif

    f.write(lengthBytes, 4);
    f.write(encryptedData.toLatin1());
    f.flush();
    f.close();
}
