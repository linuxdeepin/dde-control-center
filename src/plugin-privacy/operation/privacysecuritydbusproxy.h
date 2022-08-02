#ifndef PRIVACYSECURITYDBUSPROXY_H
#define PRIVACYSECURITYDBUSPROXY_H

#include <QObject>
#include "interface/namespace.h"

DCC_BEGIN_NAMESPACE
class DCCDBusInterface;
DCC_END_NAMESPACE

class PrivacySecurityDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityDBusProxy(QObject *parent = nullptr);

    void getPermissionInfo();

    // 设置权限信息
    void setPermissionInfo(const QString& appId, const QString& permissionGroup, const QString& permissionId, const QString& value);
    // 设置权限开关
    void getPermissionEnable(const QString& permissionGroup, const QString& permissionId);
    void setPermissionEnable(const QString& permissionGroup, const QString& permissionId, bool enable);

Q_SIGNALS:
    void PermissionEnableChanged(const QString& permissionGroup, const QString& permissionId, bool enable);
    void PermissionInfoChanged();

    void permissionEnableReset(const QString &permissionGroup, const QString &permissionId);
    void permissionInfoReset(const QString &permissionGroup, const QString &permissionId);
    // 数据加载完成
    void permissionInfoLoadFinished(const QString& perInfo);
    // 服务状态回调
    void permissionEnableLoadFinished(const bool loadState);

private:
    DCC_NAMESPACE::DCCDBusInterface *m_privacyInter;
};

#endif // SECURITYDBUSPROXY_H
