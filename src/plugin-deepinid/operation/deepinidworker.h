// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPINIDWORKER_H
#define DEEPINIDWORKER_H

#include "deepinidmodel.h"
#include "deepiniddbusproxy.h"
#include "syncdbusproxy.h"
#include "utclouddbusproxy.h"

class DeepinWorker : public QObject
{
    Q_OBJECT
public:
    explicit DeepinWorker(DeepinidModel *model, QObject *parent = nullptr);

    void initData();

    Q_INVOKABLE void loginUser();
    Q_INVOKABLE void logoutUser();
    Q_INVOKABLE void openWeb();
    Q_INVOKABLE void setFullName(const QString &name);
    Q_INVOKABLE void setAutoSync(bool autoSync);
    Q_INVOKABLE void setSyncSwitcher(const QStringList &keyList, bool enable);
    Q_INVOKABLE void setUtcloudSwitcher(const QString &key, bool enable);

    Q_INVOKABLE bool checkPasswdEmpty();
    Q_INVOKABLE QVariantMap checkPassword(const QString &passwd);
    Q_INVOKABLE void registerPasswd(const QString &passwd);
    Q_INVOKABLE void clearData();
    Q_INVOKABLE void openForgetPasswd();
    Q_INVOKABLE void unBindPlatform();
    Q_INVOKABLE void bindAccount();

public Q_SLOTS:
    void onSyncSwitcherChange(const QString &key, bool enable);
    void onLastSyncTimeChanged(qlonglong lastSyncTime);
    void licenseStateChangeSlot();

    void onUtcloudSwitcherChange(const QVariantList &args);
    void onUtcloudLoginStatusChange(const QVariantList &args);

    void onBindSuccess();

private:
    void activate();
    void requestSyncDump();
    void requestUtCloudDump();
    QString loadCodeURL();
    void getLicenseState();
    void getRSAPubKey();
    QString getSessionID();

private:
    DeepinidModel *m_model;
    DeepinidDBusProxy *m_deepinIDProxy;
    SyncDBusProxy *m_syncProxy;
    UtcloudDBusProxy *m_utcloudProxy;
    std::string m_RSApubkey;
    QDBusInterface *m_clientService;
    QString m_pwdToken;
    QString m_forgetUrl;
    QString m_wechatUrl;
};

#endif // DEEPINIDWORKER_H
