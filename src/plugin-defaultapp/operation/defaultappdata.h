// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// defaultapp 数据通道：把主线程上的 Category 状态整理成普通值快照，
// 供数据工作线程安全读取；写入请求统一编码为模块内操作。
// 本类不依赖 DefAppModel，只按类别下标消费 Category，便于独立测试。
#ifndef DEFAULTAPPDATA_H
#define DEFAULTAPPDATA_H

#include "dccfactory.h"

#include <QHash>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

class Category;
struct App;

class DefaultAppData : public QObject
{
    Q_OBJECT
public:
    // 协议键。Get/Set 只接受这些键。
    static const QStringList &supportedKeys();
    // 协议键对应的后端类别名（如 "browser" -> "Browser"）；
    // 未知键返回空串。写入路径据此调用 Worker，避免维护第二份映射。
    static QString backendNameForKey(const QString &key);

    explicit DefaultAppData(QObject *parent = nullptr);
    ~DefaultAppData() override;

    // 主线程：绑定各类别的 Category（下标与 DefAppModel::DefAppCategory 一致），
    // 并立即刷新一次快照
    void setCategories(const QHash<int, Category *> &categories);
    // 主线程：重算快照；读取状态或值变化时才会发出变更通知
    void refresh();
    // 主线程：标记通道不可用（无后端、已停用等），并保留原因
    void markUnavailable(const QString &error);
    // 任意线程：模块是否可服务请求；由逐键读取状态推导，不是独立标志
    bool isReady() const;
    void reset();
    // 任意线程：按请求键集合投影快照；请求值忽略
    dccV25::DataReply read(const QVariantMap &param) const;
    // 任意线程：校验 Set 请求（键合法、值非空），返回 Ok 时可以逐项执行
    static dccV25::DataReply validateSet(const QVariantMap &param);
    // 任意线程：用快照里的可选应用集合校验并取回 App 描述。
    // 工作线程不得直接读主线程的 Category，因此校验所需数据必须来自快照。
    bool lookupApp(const QString &key, const QString &desktopId, App *out) const;

Q_SIGNALS:
    void propertiesChanged(const QVariantMap &properties);

private:
    // 一个协议键对应的全部可见状态
    struct KeySnapshot {
        QString value;         // 后端真实默认项 Id；空串表示已确认未设置
        QVector<App> apps;     // 候选应用，供 Set 校验
        bool resolved = false; // 该键是否已成功读取
    };

    mutable QMutex m_mutex;
    // 协议键 -> 快照。就绪与否从 resolved 推导，不另设全局标志。
    QMap<QString, KeySnapshot> m_snapshot;
    QString m_error;
    QHash<int, Category *> m_categories;
    // 已连接的类别，便于 setCategories 重新登记时解除旧连接
    QList<Category *> m_connections;
};

#endif // DEFAULTAPPDATA_H
