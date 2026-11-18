// SPDX-FileCopyrightText: 2024-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "mimedbusproxyold.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>
#include <QDebug>

const QString MimeService = QStringLiteral("org.deepin.dde.Mime1");
const QString MimePath = QStringLiteral("/org/deepin/dde/Mime1");
const QString MimeInterface = QStringLiteral("org.deepin.dde.Mime1");

bool MimeDBusProxyOld::isRegisted()
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(MimeInterface);
}

namespace {

// 仍然同步等待结果（旧接口的既有行为），但保留错误信息：
// 直接转成 QString 会把 Error.Failed 退化成空串，调用方无法区分失败与未设置。
LegacyReadReply toLegacyReply(const QDBusPendingReply<QString> &reply)
{
    if (reply.isError()) {
        qWarning() << "legacy mime read failed:" << reply.error().message();
        return {};
    }
    return { true, reply.value() };
}

} // namespace

MimeDBusProxyOld::MimeDBusProxyOld(QObject *parent)
    : QObject(parent)
    , m_mimeInter(new QDBusInterface(MimeService, MimePath, MimeInterface, QDBusConnection::sessionBus(), this))
{
    connect(m_mimeInter, SIGNAL(Change()), this, SIGNAL(Change()), Qt::QueuedConnection);
}

QDBusPendingReply<void> MimeDBusProxyOld::SetDefaultApp(const QStringList &mimeTypes, const QString &desktopId)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(mimeTypes) << QVariant::fromValue(desktopId);
    return m_mimeInter->asyncCallWithArgumentList("SetDefaultApp", argumentList);
}

void MimeDBusProxyOld::DeleteApp(const QStringList &mimeTypes, const QString &desktopId)
{
    QDBusPendingReply<QString>(m_mimeInter->asyncCall("DeleteApp", mimeTypes, desktopId));
}

void MimeDBusProxyOld::DeleteUserApp(const QString &desktopId)
{
    QDBusPendingReply<QString>(m_mimeInter->asyncCall("DeleteUserApp", desktopId));
}

void MimeDBusProxyOld::AddUserApp(const QStringList &mimeTypes, const QString &desktopId)
{
    QDBusPendingReply<QString>(m_mimeInter->asyncCall("AddUserApp", mimeTypes, desktopId));
}

LegacyReadReply MimeDBusProxyOld::GetDefaultApp(const QString &mimeType)
{
    // 保持同步阻塞语义，但不再把错误吞成空串
    return toLegacyReply(QDBusPendingReply<QString>(m_mimeInter->asyncCall("GetDefaultApp", mimeType)));
}

LegacyReadReply MimeDBusProxyOld::ListApps(const QString &mimeType)
{
    return toLegacyReply(QDBusPendingReply<QString>(m_mimeInter->asyncCall("ListApps", mimeType)));
}

LegacyReadReply MimeDBusProxyOld::ListUserApps(const QString &mimeType)
{
    return toLegacyReply(QDBusPendingReply<QString>(m_mimeInter->asyncCall("ListUserApps", mimeType)));
}
