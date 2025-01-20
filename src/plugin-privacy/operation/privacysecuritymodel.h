// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractProxyModel>
#include "applicationitem.h"

class AppsModel : public QAbstractListModel
{
    Q_OBJECT
    enum AppPrivacyRole {
        NameRole = Qt::UserRole + 1,
        IconNameRole,

        CameraRole,
        DocumentRole,
        PictureRole,
        DesktopRole,
        VideoRole,
        MusicRole,
        DownloadRole,
    };
public:
    explicit AppsModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, const QString propertyName = QString()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    ApplicationItemListPtr appList() const { return m_appItems; };
    void reset(ApplicationItemListPtr appList);
    void appendItem(ApplicationItem *item);
    void removeItem(ApplicationItem *item);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    ApplicationItemListPtr m_appItems;
};

class PrivacySecurityModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AppsModel *appModel MEMBER m_appModel CONSTANT)
public:
    explicit PrivacySecurityModel(QObject *parent = nullptr);
    ~PrivacySecurityModel();

    AppsModel *appModel() { return m_appModel; }

    bool isPremissionEnabled(int premission) const;
    void setPremissionEnabled(int premission, bool enabled);

    ApplicationItem *applictionItem(unsigned uniqueID);
    int getAppIndex(unsigned uniqueID) const;
    void updatePermission();
    bool updatePermission(ApplicationItem *item);
    const QSet<QString> blacklist(const QString &file) const;

    const QString premissiontoPath(int premission) const;
    int pathtoPremission(const QString &path, bool mainPremission) const;

    // 数据更新中，界面应禁用
    inline bool updating() const { return m_updating; }

    inline unsigned lastUniqueID() const { return m_uniqueID; }

Q_SIGNALS:
    void serviceExistsChanged(bool exists);
    void checkAuthorization(bool checking);
    void premissionEnabledChanged(int premission, bool enabled);
    void requestSetPremissionMode(int premission, int mode);
    void requestUpdateCacheBlacklist(const QMap<QString, QSet<QString>> &cacheBlacklist);

    void itemAboutToBeAdded(int pos);
    void itemAdded();
    void itemAboutToBeRemoved(int pos);
    void itemRemoved();
    void itemDataChanged(ApplicationItem *appItem);
    void itemDataUpdate(bool updating);

public Q_SLOTS:
    void emitAppDataChanged(ApplicationItem *item);

protected Q_SLOTS:
    void onPremissionModeChanged(int premission, int mode);
    void emitPremissionModeChanged(int premission);
    void onAppPremissionEnabledChanged(const QString &file, const QSet<QString> &apps);
    void onItemPermissionChanged();
    void onItemDataChanged();
    void onCacheBlacklistChanged(const QMap<QString, QSet<QString>> &cacheBlacklist);

    bool addApplictionItem(ApplicationItem *item);
    void removeApplictionItem(const QString &id);
    void dataUpdateFinished(bool updating);

private:
    unsigned createUniqueID();

private:
    AppsModel *m_appModel = nullptr;
    unsigned m_uniqueID;                           // ApplicationItem的唯一ID，从1开始，只处理m_appItems中的项
    QMap<int, int> m_premissionMap;                // 权限总开关
    QMap<QString, QSet<QString>> m_blacklist;      // 权限列表
    QMap<QString, QSet<QString>> m_cacheBlacklist; // 权限列表
    bool m_updating;                               // 数据更新中

    friend class PrivacySecurityWorker;
};
