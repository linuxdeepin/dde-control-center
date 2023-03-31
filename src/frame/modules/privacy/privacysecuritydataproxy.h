// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef PRIVACYSECURITYDATAPROXY_H
#define PRIVACYSECURITYDATAPROXY_H

#include <QObject>

struct AppItemInfo
{
    QString Path;
    QString Name;
    QString ID;
    QString Icon;
    qint64 CategoryID;
    qint64 TimeInstalled;
};
Q_DECLARE_METATYPE(AppItemInfo)
typedef QList<AppItemInfo> AppItemInfoList;
Q_DECLARE_METATYPE(AppItemInfoList)

namespace Dtk {
namespace Core {
class DConfig;
}
}
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
    void fileArmorExistsChanged(bool exists);
    void itemInfosChanged(const AppItemInfoList &itemList);
    void itemChanged(const QString &status, AppItemInfo itemInfo, qlonglong categoryID);

    void fileEnableChanged(const QString &file, const QStringList &apps, bool enable);
    void fileListChanged(const QStringList &files);
    void fileAppsChanged(const QString &file, const QPair<QStringList, bool> &apps);
    void fileModeChanged(const QString &file, int mode);

    void cameraEnableChanged(const QStringList &apps, bool enable);
    void cameraAppsChanged(const QPair<QStringList, bool> &apps);
    void cameraModeChanged(int mode);

public Q_SLOTS:
    void init();
    // Launcherd
    void getAllItemInfos();
    // FileArmor
    bool existsFileArmor() const;

    void fileEnable(const QString &file, const QStringList &apps, bool enable);
    void fileList();
    void fileGetApps(const QString &file);
    void fileSetMode(const QString &file, int mode);
    void fileGetMode(const QString &file);

    // Camera
    void cameraEnable(const QStringList &apps, bool enable);
    void cameraGetApps();
    void cameraSetMode(int mode);
    void cameraGetMode();

    // cacheBlacklist
    QMap<QString, QStringList> getCacheBlacklist();
    void setCacheBlacklist(const QMap<QString, QStringList> &cacheBlacklist);
    // appInfo
    // 根据文件获取包中所有文件
    QStringList getExecutable(const QString &path);
    QMap<QString, QStringList> getPackagesExecutable(const QStringList &paths);

private Q_SLOTS:
    void onFileArmorChanged();
    void onGetItemInfosFinished(QDBusPendingCallWatcher *w);

    void onFileEnableFinished(QDBusPendingCallWatcher *w);
    void onFileListFinished(QDBusPendingCallWatcher *w);
    void onFileGetAppsFinished(QDBusPendingCallWatcher *w);
    void onFileSetModeFinished(QDBusPendingCallWatcher *w);
    void onFileGetModeFinished(QDBusPendingCallWatcher *w);

    void onCameraEnableFinished(QDBusPendingCallWatcher *w);
    void onCameraGetAppsFinished(QDBusPendingCallWatcher *w);
    void onCameraSetModeFinished(QDBusPendingCallWatcher *w);
    void onCameraGetModeFinished(QDBusPendingCallWatcher *w);

    void initModstatdb();
    void shutdownModstatdb();

private:
    Dtk::Core::DConfig *m_dconfig;
    bool m_initModstatdb; // dpkg 初始化标志
    QFileSystemWatcher *m_pSystemWatcher;
};

#endif // PRIVACYSECURITYDATAPROXY_H
