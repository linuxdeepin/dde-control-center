// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 数据通道请求层：负责把一次 DBus 调用变成一次且仅一次回复。
// 对外错误名与对外签名固定在这里，插件只返回业务结果。
#ifndef DCCDATAREQUEST_H
#define DCCDATAREQUEST_H

#include "dccfactory.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QSet>
#include <QObject>
#include <QString>

namespace dccV25 {

enum class DataOperation { Get, Set };

// 访问策略只在框架内部使用，不进入公共 SDK
class DataAccessPolicy
{
public:
    virtual ~DataAccessPolicy() = default;
    // caller 为 DBus 请求的真实唯一总线名，不接受请求中自称的身份
    virtual bool isAllowed(const QString &module, DataOperation op, const QString &caller) const = 0;
};

// 默认白名单策略：Set 在名单为空时一律拒绝（fail closed）
class WhitelistAccessPolicy : public DataAccessPolicy
{
public:
    void allowModule(const QString &module);
    bool isAllowed(const QString &module, DataOperation op, const QString &caller) const override;

private:
    QSet<QString> m_modules;
};

// 单次请求的完整上下文。只在主线程创建、修改与完成
class DataRequest : public QObject
{
    Q_OBJECT
public:
    // 计划 §6.3 的初始边界：一次请求 20 秒，全局 64 个未完成，单模块 8 个
    static constexpr int kTimeoutMs = 20000;
    static constexpr int kMaxPendingTotal = 64;
    static constexpr int kMaxPendingPerModule = 8;
    // 请求顶层键数上限，避免异常载荷拖垮处理与日志
    static constexpr int kMaxParamKeys = 16;

    DataRequest(quint64 id, QString module, DataOperation op,
                QVariantMap param, QString caller, QString callerPath,
                QDBusConnection connection, QDBusMessage message,
                QObject *parent = nullptr);

    quint64 id() const { return m_id; }
    const QString &module() const { return m_module; }
    DataOperation operation() const { return m_op; }
    const QVariantMap &param() const { return m_param; }
    const QString &caller() const { return m_caller; }
    // 调用者可执行文件路径；解析失败时为空串，表示未知
    const QString &callerPath() const { return m_callerPath; }
    bool isFinished() const { return m_finished; }

    // 唯一回复出口：成功/失败都经此，重复调用无效
    void finish(const QVariantList &arguments);
    void finishWithError(const QString &name, const QString &message);
    void finishFromReply(const DataReply &reply);

private:
    quint64 m_id;
    QString m_module;
    DataOperation m_op;
    QVariantMap m_param;
    QString m_caller;
    QString m_callerPath;
    QDBusConnection m_connection;
    QDBusMessage m_message;
    bool m_finished = false;
};

} // namespace dccV25

#endif // DCCDATAREQUEST_H
