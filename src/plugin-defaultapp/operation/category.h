// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CATEGORY_H
#define CATEGORY_H

#include <QObject>
#include <QList>

struct App {
    QString dbusPath;
    QString Id;
    QString Name;
    QString DisplayName;
    QString Description;
    QString Icon;
    QString Exec;
    bool isUser;
    bool CanDelete;
    bool MimeTypeFit;

    App() : isUser(false), CanDelete(false), MimeTypeFit(false) {}

    bool operator ==(const App &app) const {
        return app.Id == Id && app.isUser == isUser;
    }

    bool operator !=(const App &app) const {
        return app.Id != Id && app.isUser != isUser;
    }
};

class Category : public QObject
{
    Q_OBJECT
public:
    // 一个类别相对后端的读取状态。
    // 只有 Ok 表示"已知后端的真实值"：Unread 与 Failed 都不能当作"已确认未设置"，
    // 否则读取失败会被伪装成合法的空值。
    enum class ReadState : quint8 {
        Unread = 0, // 尚未读取或读取尚未结束
        Ok,         // 读取成功（含"已确认未设置"，此时 backendDefault().Id 为空）
        Failed,     // 读取失败，可重试
    };
    Q_ENUM(ReadState)

    explicit Category(QObject *parent = Q_NULLPTR);

    // 写入/读取成功后调用：同时更新后端真实值与显示默认项
    void setDefault(const App &def);
    // 读取路径：只记录后端真实值，并把状态置为 Ok
    void setBackendDefault(const App &def);
    // 仅更新显示默认项（UI 回退），不影响后端真实值与读取状态
    void setDisplayDefault(const App &def);

    void setReadState(ReadState state);
    ReadState readState() const { return m_readState; }
    bool isResolved() const { return m_readState == ReadState::Ok; }

    const QString getName() const { return m_category;}
    void setCategory(const QString &category);
    inline const QList<App> getappItem() const { return m_applist;}
    inline const QList<App> systemAppList() const { return m_systemAppList; }
    inline const QList<App> userAppList() const { return m_userAppList; }
    inline const App getDefault() const { return m_default;}
    // 后端返回的真实默认项；Id 为空表示已确认未设置。
    // 显示默认项可能是 UI 回退值，不能用于数据通道。
    inline const App &backendDefault() const { return m_backendDefault; }
    void clear();
    void addUserItem(const App &value);
    void delUserItem(const App &value);

Q_SIGNALS:
    void defaultChanged(const App &id);
    void backendDefaultChanged(const App &app);
    // 读取状态变化：Unread→Ok/Failed、Failed→Ok，以及重试前的 Ok→Unread
    void readStateChanged(Category::ReadState state);
    void addedUserItem(const App &app);
    void removedUserItem(const App &app);
    void categoryNameChanged(const QString &name);
    void clearAll();

private:
    QList<App> m_applist;
    QList<App> m_systemAppList;
    QList<App> m_userAppList;
    QString m_category;
    App m_default;
    App m_backendDefault;
    ReadState m_readState = ReadState::Unread;
};
#endif // CATEGORY_H
