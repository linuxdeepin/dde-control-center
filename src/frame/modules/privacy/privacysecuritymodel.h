// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef PRIVACYSECURITYMODEL_H
#define PRIVACYSECURITYMODEL_H

#include <QObject>
#include <QMap>

class ServiceControlItems;
class ApplicationItem;

// 翻译仅在界面类中
class PrivacySecurityModel : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityModel(QObject *parent = nullptr);

    bool isPremissionEnabled(int premission) const;
    void setPremissionEnabled(int premission, bool enabled);

    inline const QList<ApplicationItem *> &applictionItems() const { return m_appItems; }
    ApplicationItem *applictionItem(unsigned uniqueID);
    int getAppIndex(unsigned uniqueID) const;
    void updatePermission();
    void updatePermission(ApplicationItem *item);
    const QStringList blacklist(const QString &file) const;

    const QString premissiontoPath(int premission) const;
    int pathtoPremission(const QString &path, bool mainPremission) const;

    inline bool updating() const { return m_updating; }

Q_SIGNALS:
    void premissionEnabledChanged(int premission, bool enabled);
    void requestSetPremissionMode(int premission, int mode);
    void requestUpdateCacheBlacklist(const QMap<QString, QStringList> &cacheBlacklist);

    void itemAboutToBeAdded(int pos);
    void itemAdded();
    void itemAboutToBeRemoved(int pos);
    void itemRemoved();
    void itemDataChanged(ApplicationItem *appItem);
    void itemDataUpdate(bool updating);

protected Q_SLOTS:
    void onPremissionModeChanged(int premission, int mode);
    void onAppPremissionEnabledChanged(const QString &file, const QStringList &apps);
    void onItemDataChanged();
    void onCacheBlacklistChanged(const QMap<QString, QStringList> &cacheBlacklist);

    bool addApplictionItem(ApplicationItem *item);
    void removeApplictionItem(const QString &id);
    void dataUpdateFinished(bool updating);

private:
    unsigned createUniqueID();

private:
    QList<ApplicationItem *> m_appItems;
    unsigned m_uniqueID;                         // ApplicationItem的唯一ID，从1开始，只处理m_appItems中的项
    QMap<int, int> m_premissionMap;              // 权限总开关
    QMap<QString, QStringList> m_blacklist;      // 权限列表
    QMap<QString, QStringList> m_cacheBlacklist; // 权限列表
    bool m_updating;                             // 数据更新中

    friend class PrivacySecurityWorker;
};

#endif // PRIVACYSECURITYMODEL_H
