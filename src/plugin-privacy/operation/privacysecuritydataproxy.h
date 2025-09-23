// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PRIVACYSECURITYDATAPROXY_H
#define PRIVACYSECURITYDATAPROXY_H

#include <QLoggingCategory>
#include <QObject>

Q_DECLARE_LOGGING_CATEGORY(DCC_PRIVACY)

namespace Dtk {
namespace Core {
class DConfig;
} // namespace Core
} // namespace Dtk
class QDBusPendingCallWatcher;
class QFileSystemWatcher;

class PrivacySecurityDataProxy : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityDataProxy(QObject *parent = nullptr);
    ~PrivacySecurityDataProxy();

    enum Mode {
        AllDisabled = 0,
        AllEnable = 1,
        ByCustom = 2,
    };

Q_SIGNALS:
    void serviceExistsChanged(bool exists);

    void PolicyChanged(const QString &policy, const QString &type);
    void EntityChanged(const QString &entity, const QString &type);
    void ModeChanged(const QString &mode, const QString &type);

public Q_SLOTS:
    void getMode(const QString &object);
    void setMode(const QString &config);
    void listEntity();
    void getEntity(const QString &entity);
    void setEntity(const QString &entity);
    void getPolicy(const QString &entity);
    void setPolicy(const QString &policy);

public Q_SLOTS:
    void init();
    // FileArmor
    bool existsService() const;

    // cacheBlacklist
    QMap<QString, QSet<QString>> getCacheBlacklist();
    void setCacheBlacklist(const QMap<QString, QSet<QString>> &cacheBlacklist);
    // appInfo
    // 根据文件获取包中所有文件
    QStringList getExecutable(const QString &path, QString *package);

private Q_SLOTS:
    void onGetNameOwner(const QString &);
    void onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);
    void updateServiceExists(bool serviceExists);

    void onGetModeFinished(QDBusPendingCallWatcher *w);
    void onSetModeFinished(QDBusPendingCallWatcher *w);
    void onListEntityFinished(QDBusPendingCallWatcher *w);
    void onGetEntityFinished(QDBusPendingCallWatcher *w);
    void onSetEntityFinished(QDBusPendingCallWatcher *w);
    void onGetPolicyFinished(QDBusPendingCallWatcher *w);
    void onSetPolicyFinished(QDBusPendingCallWatcher *w);

private:
    bool m_serviceExists;
    Dtk::Core::DConfig *m_dconfig;
};

#endif // PRIVACYSECURITYDATAPROXY_H
