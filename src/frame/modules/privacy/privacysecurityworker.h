// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef PRIVACYSECURITYWORKER_H
#define PRIVACYSECURITYWORKER_H

#include <QObject>
#include "privacysecuritymodel.h"

class PrivacySecurityDataProxy;
class QDBusPendingCallWatcher;
struct AppItemInfo;
typedef QList<AppItemInfo> AppItemInfoList;
class PrivacySecurityWorker : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityWorker(PrivacySecurityModel *model, QObject *parent = nullptr);
    ~PrivacySecurityWorker();

    void deactivate();

public Q_SLOTS:
    void activate();
    // 设置总开关
    void setPermissionMode(int premission, int mode);
    void setAppPermissionEnable(int premission, bool enabled, ApplicationItem *item);
    void checkAuthorizationCancel();
    void updateCacheBlacklist(const QMap<QString, QStringList> &cacheBlacklist);

private:
    QString getAppPath(const QString &filePath);
    void updateCheckAuthorizationing(bool checking);

private Q_SLOTS:
    void onItemInfosChanged(const AppItemInfoList &itemList);
    void onItemChanged(const QString &status, const AppItemInfo &itemInfo, qlonglong categoryID);
    ApplicationItem *addAppItem(const AppItemInfo &itemInfo);
    void onFileAppsChanged(const QString &file, const QPair<QStringList, bool> &apps);
    void onFileModeChanged(const QString &file, int mode);
    void onCameraAppsChanged(const QPair<QStringList, bool> &apps);
    void onCameraModeChanged(int mode);

Q_SIGNALS:
    void checkAuthorization(bool checking);

private:
    PrivacySecurityModel *m_model;
    PrivacySecurityDataProxy *m_dataProxy;
    QStringList m_pathList;
    bool m_checkAuthorizationing;
    QList<QPair<ApplicationItem *, QPair<int, bool>>> m_cacheAppPermission;
};

#endif // PRIVACYSECURITYWORKER_H
