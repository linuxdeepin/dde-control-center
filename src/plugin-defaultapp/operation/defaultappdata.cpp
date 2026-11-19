// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "defaultappdata.h"

#include "category.h"

#include <QMutexLocker>
#include <QVector>

namespace {

// 唯一的键定义：协议键、模型类别下标、后端类别名在这里一一对应。
// supportedKeys()、快照刷新、Set 写入映射都从这里派生，不再各自维护一份。
struct KeyInfo {
    const char *key;
    int category; // 与 DefAppModel::DefAppCategory 一致
    const char *backendName;
};

const QVector<KeyInfo> &keyTable()
{
    static const QVector<KeyInfo> table = {
        { "browser", 0, "Browser" },
        { "mail", 1, "Mail" },
        { "text", 2, "Text" },
        { "music", 3, "Music" },
        { "video", 4, "Video" },
        { "picture", 5, "Picture" },
        { "terminal", 6, "Terminal" },
    };
    return table;
}

const KeyInfo *keyInfo(const QString &key)
{
    for (const auto &info : keyTable()) {
        if (key == QLatin1String(info.key)) {
            return &info;
        }
    }
    return nullptr;
}

} // namespace

const QStringList &DefaultAppData::supportedKeys()
{
    static const QStringList keys = [] {
        QStringList list;
        list.reserve(keyTable().size());
        for (const auto &info : keyTable()) {
            list.append(QLatin1String(info.key));
        }
        return list;
    }();
    return keys;
}

QString DefaultAppData::backendNameForKey(const QString &key)
{
    const KeyInfo *info = keyInfo(key);
    return info ? QString::fromLatin1(info->backendName) : QString();
}

DefaultAppData::DefaultAppData(QObject *parent)
    : QObject(parent)
{
}

DefaultAppData::~DefaultAppData() = default;
void DefaultAppData::setCategories(const QHash<int, Category *> &categories)
{
    for (QObject *connection : std::as_const(m_connections)) {
        disconnect(connection, nullptr, this, nullptr);
    }
    m_connections.clear();

    m_categories = categories;
    for (Category *category : std::as_const(m_categories)) {
        if (!category) {
            continue;
        }
        // 快照自己跟踪来源状态：持有者只需登记一次，忘记连接也不会让快照变陈旧
        connect(category, &Category::backendDefaultChanged, this, &DefaultAppData::refresh);
        connect(category, &Category::defaultChanged, this, &DefaultAppData::refresh);
        connect(category, &Category::readStateChanged, this, &DefaultAppData::refresh);
        connect(category, &Category::addedUserItem, this, &DefaultAppData::refresh);
        connect(category, &Category::removedUserItem, this, &DefaultAppData::refresh);
        m_connections.append(category);
    }
    refresh();
}

void DefaultAppData::markUnavailable(const QString &error)
{
    QMutexLocker locker(&m_mutex);
    m_error = error;
    // 不可用不代表"已确认未设置"：清掉 resolved，读取请求应得到 NotReady
    for (auto it = m_snapshot.begin(); it != m_snapshot.end(); ++it) {
        it->resolved = false;
    }
}

void DefaultAppData::reset()
{
    QMutexLocker locker(&m_mutex);
    m_snapshot.clear();
    m_error.clear();
}

bool DefaultAppData::isReady() const
{
    QMutexLocker locker(&m_mutex);
    if (!m_error.isEmpty() || m_snapshot.size() != keyTable().size()) {
        return false;
    }
    for (auto it = m_snapshot.cbegin(); it != m_snapshot.cend(); ++it) {
        if (!it->resolved) {
            return false;
        }
    }
    return true;
}

void DefaultAppData::refresh()
{
    if (m_categories.isEmpty()) {
        return;
    }

    // 只在主线程调用：把每个类别的状态摊平成普通值快照
    QMap<QString, KeySnapshot> fresh;
    for (const auto &info : keyTable()) {
        const auto cat = m_categories.constFind(info.category);
        KeySnapshot snapshot;
        if (cat != m_categories.cend() && cat.value()) {
            // 默认值取后端真实值，不是 getDefault()：后者可能是 UI 回退项
            snapshot.value = cat.value()->backendDefault().Id;
            snapshot.apps = cat.value()->getappItem().toVector();
            snapshot.resolved = cat.value()->isResolved();
        }
        fresh.insert(QLatin1String(info.key), snapshot);
    }

    QVariantMap changed;
    {
        QMutexLocker locker(&m_mutex);
        // 只有值真正变化才通知：旧值与新值都必须来自"已读取成功"的状态，
        // 否则未读取到已读取的过渡会被当成用户可见变更。
        for (auto it = fresh.cbegin(); it != fresh.cend(); ++it) {
            const auto old = m_snapshot.constFind(it.key());
            if (old != m_snapshot.cend() && old->resolved && it->resolved
                && old->value != it->value) {
                changed.insert(it.key(), it->value);
            }
        }
        m_snapshot = fresh;
    }
    if (!changed.isEmpty()) {
        Q_EMIT propertiesChanged(changed);
    }
}

dccV25::DataReply DefaultAppData::read(const QVariantMap &param) const
{
    if (param.isEmpty()) {
        return { dccV25::DataStatus::InvalidArgs, {}, QStringLiteral("empty request") };
    }

    // 请求值忽略，只取键集合；未知键整体拒绝
    QStringList requested;
    for (auto it = param.cbegin(); it != param.cend(); ++it) {
        if (!keyInfo(it.key())) {
            return { dccV25::DataStatus::InvalidArgs, {},
                     QStringLiteral("unknown key: ") + it.key() };
        }
        requested.append(it.key());
    }

    QMap<QString, KeySnapshot> snapshot;
    QString backendError;
    {
        QMutexLocker locker(&m_mutex);
        snapshot = m_snapshot;
        backendError = m_error;
    }

    QVariantMap result;
    for (const QString &key : requested) {
        const auto it = snapshot.constFind(key);
        // 未读取、读取失败、或整个通道不可用：都不能返回空值冒充"未设置"
        if (it == snapshot.cend() || !it->resolved) {
            return { dccV25::DataStatus::NotReady, {},
                     backendError.isEmpty()
                             ? QStringLiteral("key not ready: ") + key
                             : backendError };
        }
        result.insert(key, it->value);
    }
    return { dccV25::DataStatus::Ok, result, {} };
}

dccV25::DataReply DefaultAppData::validateSet(const QVariantMap &param)
{
    if (param.isEmpty()) {
        return { dccV25::DataStatus::InvalidArgs, {}, QStringLiteral("empty request") };
    }
    for (auto it = param.cbegin(); it != param.cend(); ++it) {
        if (!keyInfo(it.key())) {
            return { dccV25::DataStatus::InvalidArgs, {},
                     QStringLiteral("unknown key: ") + it.key() };
        }
        // 值不做类型限制，但不能为空：空值表示“未设置”，不能作为写入目标
        if (it.value().toString().isEmpty()) {
            return { dccV25::DataStatus::InvalidArgs, {},
                     QStringLiteral("empty value for key: ") + it.key() };
        }
    }
    return { dccV25::DataStatus::Ok, {}, {} };
}

bool DefaultAppData::lookupApp(const QString &key, const QString &desktopId, App *out) const
{
    if (!out || desktopId.isEmpty()) {
        return false;
    }
    QVector<App> candidates;
    {
        QMutexLocker locker(&m_mutex);
        const auto it = m_snapshot.constFind(key);
        // 只有已成功读取的键才有可信的候选集合
        if (it == m_snapshot.cend() || !it->resolved) {
            return false;
        }
        candidates = it->apps;
    }
    for (const App &app : candidates) {
        if (app.Id == desktopId) {
            *out = app;
            return true;
        }
    }
    return false;
}
