// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccdatarequest.h"

#include <QDBusError>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(dccLogC, "org.deepin.dde.control-center.datachannel")

namespace dccV25 {

namespace {

const QString ErrorNotReady = QStringLiteral("org.deepin.dde.ControlCenter1.Error.NotReady");
const QString ErrorCancelled = QStringLiteral("org.deepin.dde.ControlCenter1.Error.Cancelled");

QString defaultMessage(DataStatus status)
{
    switch (status) {
    case DataStatus::Ok: return QString();
    case DataStatus::NotSupported: return QStringLiteral("module does not provide a data channel");
    case DataStatus::InvalidArgs: return QStringLiteral("invalid request arguments");
    case DataStatus::NotReady: return QStringLiteral("module backend is not ready");
    case DataStatus::AccessDenied: return QStringLiteral("access denied");
    case DataStatus::Failed: return QStringLiteral("backend operation failed");
    case DataStatus::NotFound: return QStringLiteral("module not found");
    }
    return QStringLiteral("unknown error");
}

} // namespace

void WhitelistAccessPolicy::allowModule(const QString &module)
{
    m_modules.insert(module);
}

bool WhitelistAccessPolicy::isAllowed(const QString &module, DataOperation op, const QString &caller) const
{
    Q_UNUSED(caller)
    if (op == DataOperation::Get) {
        // 读取默认放行，先落地读取能力；写入必须显式列入白名单
        return true;
    }
    return m_modules.contains(module);
}

DataRequest::DataRequest(quint64 id, QString module, DataOperation op,
                         QVariantMap param, QString caller, QString callerPath,
                         QDBusConnection connection, QDBusMessage message,
                         QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_module(std::move(module))
    , m_op(op)
    , m_param(std::move(param))
    , m_caller(std::move(caller))
    , m_callerPath(std::move(callerPath))
    , m_connection(std::move(connection))
    , m_message(std::move(message))
{
}

void DataRequest::finish(const QVariantList &arguments)
{
    if (m_finished) {
        return;
    }
    m_finished = true;
    if (!m_connection.send(m_message.createReply(arguments))) {
        qCWarning(dccLogC) << "failed to send data reply for" << m_module;
    }
    deleteLater();
}

void DataRequest::finishWithError(const QString &name, const QString &message)
{
    if (m_finished) {
        return;
    }
    m_finished = true;
    if (!m_connection.send(m_message.createErrorReply(name, message))) {
        qCWarning(dccLogC) << "failed to send data error for" << m_module << name;
    }
    deleteLater();
}

void DataRequest::finishFromReply(const DataReply &reply)
{
    switch (reply.status) {
    case DataStatus::Ok:
        // Get 返回请求键到结果的映射；Set 返回逐项结果（空值表示成功）
        finish({ QVariant(reply.value) });
        break;
    case DataStatus::NotSupported:
        finishWithError(QStringLiteral("org.freedesktop.DBus.Error.NotSupported"),
                        reply.message.isEmpty() ? defaultMessage(reply.status) : reply.message);
        break;
    case DataStatus::InvalidArgs:
        finishWithError(QStringLiteral("org.freedesktop.DBus.Error.InvalidArgs"),
                        reply.message.isEmpty() ? defaultMessage(reply.status) : reply.message);
        break;
    case DataStatus::AccessDenied:
        finishWithError(QStringLiteral("org.freedesktop.DBus.Error.AccessDenied"),
                        reply.message.isEmpty() ? defaultMessage(reply.status) : reply.message);
        break;
    case DataStatus::NotReady:
        finishWithError(ErrorNotReady,
                        reply.message.isEmpty() ? defaultMessage(reply.status) : reply.message);
        break;
    case DataStatus::Failed:
        finishWithError(QStringLiteral("org.freedesktop.DBus.Error.Failed"),
                        reply.message.isEmpty() ? defaultMessage(reply.status) : reply.message);
        break;
    case DataStatus::NotFound:
        finishWithError(QStringLiteral("org.deepin.dde.ControlCenter1.Error.NotFound"),
                        reply.message.isEmpty() ? defaultMessage(reply.status) : reply.message);
        break;
    }
}

} // namespace dccV25
