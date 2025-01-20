// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "privacysecuritymodel.h"
#include "privacysecuritydataproxy.h"
#include "applicationitem.h"
#include <QMutex>

class PrivacySecurityWorker : public QObject {
    Q_OBJECT
public:
    explicit PrivacySecurityWorker(PrivacySecurityModel *appModel, QObject *parent = nullptr);
    ~PrivacySecurityWorker();

public slots:
    void setPremissionEnabled(int appItemIndex, int premission, bool enabled);

    void setAppPermissionEnable(int premission, bool enabled, ApplicationItem *item);
    void checkAuthorizationCancel();
    void updateCacheBlacklist(const QMap<QString, QSet<QString>> &cacheBlacklist);

private slots:
    void updateAppPath(ApplicationItem *item);
    ApplicationItem *addAppItem(int index);

    void onModeChanged(const QString &mode, const QString &type);
    void onEntityChanged(const QString &entity, const QString &type);
    void onPolicyChanged(const QString &policy, const QString &type);

    void setAppPermissionEnableByCheck(bool ok);

signals:
    void checkAuthorization(bool checking);
    void serviceExistsChanged(bool exists);

    void appAdded(const QString &appId);
    void appRemoved(const QString &id);

private:
    void updateAllPermission();
    QString getAppPath(const QMap<QString, QString> &execs);
    void updateCheckAuthorizationing(bool checking);
    QString getEntityJson(const QString &name, bool isFile);
    QString getAppEntityJson(const ApplicationItem *item);
    QString getSubjectModeJson(const QString &name, bool isBlacklist);
    QString getObjectPolicyJson(const ApplicationItem *item, int premission, bool enabled);

    bool existsService() const;

    void init();
    void initApp();

private:
    PrivacySecurityModel *m_model = nullptr;
    QAbstractItemModel *m_ddeAmModel = nullptr;
    PrivacySecurityDataProxy *m_dataProxy = nullptr;
    QStringList m_pathList;
    bool m_checkAuthorizationing = false;
    QList<QPair<ApplicationItem *, QPair<int, bool>>> m_cacheAppPermission;
    QList<QPair<int, int>> m_cachePermission;
    QHash<QString, QSet<QString>> m_entityMap;         // entity信息,对包是 <包名,可执行文件列表>
    QMap<QString, QSet<QString>> m_blacklistByPackage; // 黑名单 <权限，包名列表>

    QMutex m_appItemsMutex;
};
