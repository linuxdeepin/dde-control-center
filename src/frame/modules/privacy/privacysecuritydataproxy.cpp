// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "privacysecuritydataproxy.h"

#include <DConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>
#include <QFileInfo>

extern "C" {
#define LIBDPKG_VOLATILE_API 1
#include <dpkg/dpkg.h>
#include <dpkg/dpkg-db.h>
#include <dpkg/db-fsys.h>
}

#define DBUS_TIMEOUT 10000

const QString LauncherService = "com.deepin.dde.daemon.Launcher";
const QString LauncherPath = "/com/deepin/dde/daemon/Launcher";
const QString LauncherInterface = "com.deepin.dde.daemon.Launcher";

const QString FileArmorService = "com.deepin.FileArmor1";
const QString FileArmorPath = "/com/deepin/FileArmor1";
const QString FileArmorInterface = "com.deepin.FileArmor1";

const QString CameraControlService = "com.deepin.FileArmor1";
const QString CameraControlPath = "/com/deepin/FileArmor1/CameraControl";
const QString CameraControlInterface = "com.deepin.FileArmor1.CameraControl";

QDBusArgument &operator<<(QDBusArgument &argument, const AppItemInfo &versionInfo)
{
    argument.beginStructure();
    argument << versionInfo.Path;
    argument << versionInfo.Name;
    argument << versionInfo.ID;
    argument << versionInfo.Icon;
    argument << versionInfo.CategoryID;
    argument << versionInfo.TimeInstalled;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, AppItemInfo &versionInfo)
{
    argument.beginStructure();
    argument >> versionInfo.Path;
    argument >> versionInfo.Name;
    argument >> versionInfo.ID;
    argument >> versionInfo.Icon;
    argument >> versionInfo.CategoryID;
    argument >> versionInfo.TimeInstalled;
    argument.endStructure();
    return argument;
}

PrivacySecurityDataProxy::PrivacySecurityDataProxy(QObject *parent)
    : QObject(parent)
    , m_dconfig(Dtk::Core::DConfig::create("org.deepin.dde.control-center", "org.deepin.dde.control-center.privacy"))
{
    qDBusRegisterMetaType<AppItemInfo>();
    qDBusRegisterMetaType<AppItemInfoList>();
    dpkg_program_init("dde-control-center");
    QDBusConnection::sessionBus().connect(LauncherService, LauncherPath, LauncherInterface, "ItemChanged", this, SLOT(itemChanged(const QString &, AppItemInfo, qlonglong)));
}

PrivacySecurityDataProxy::~PrivacySecurityDataProxy()
{
    dpkg_program_done();
}

void PrivacySecurityDataProxy::getAllItemInfos()
{
    QDBusMessage message = QDBusMessage::createMethodCall(LauncherService, LauncherPath, LauncherInterface, "GetAllItemInfos");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(message, DBUS_TIMEOUT), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onGetItemInfosFinished);
}

void PrivacySecurityDataProxy::fileEnable(const QString &file, const QStringList &apps, bool enable)
{
    // FileArmor不支持apps为空
    if (apps.isEmpty()) {
        fileSetMode(file, enable ? PrivacySecurityDataProxy::AllDisabled : PrivacySecurityDataProxy::AllEnable);
    } else {
        QDBusMessage message = QDBusMessage::createMethodCall(FileArmorService, FileArmorPath, FileArmorInterface, "Enable");
        message << file << apps << enable;
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
        watcher->setProperty("DBusFile", file);
        watcher->setProperty("DBusApps", apps);
        watcher->setProperty("DBusEnable", enable);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onFileEnableFinished);
    }
}

void PrivacySecurityDataProxy::fileList()
{
    QDBusMessage message = QDBusMessage::createMethodCall(FileArmorService, FileArmorPath, FileArmorInterface, "List");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onFileListFinished);
}

void PrivacySecurityDataProxy::fileGetApps(const QString &file)
{
    QDBusMessage message = QDBusMessage::createMethodCall(FileArmorService, FileArmorPath, FileArmorInterface, "GetApps");
    message << file;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusFile", file);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onFileGetAppsFinished);
}

void PrivacySecurityDataProxy::fileSetMode(const QString &file, int mode)
{
    QDBusMessage message = QDBusMessage::createMethodCall(FileArmorService, FileArmorPath, FileArmorInterface, "SetMode");
    message << file << mode;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusFile", file);
    watcher->setProperty("DBusMode", mode);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onFileSetModeFinished);
}

void PrivacySecurityDataProxy::fileGetMode(const QString &file)
{
    QDBusMessage message = QDBusMessage::createMethodCall(FileArmorService, FileArmorPath, FileArmorInterface, "GetMode");
    message << file;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusFile", file);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onFileGetModeFinished);
}

void PrivacySecurityDataProxy::cameraEnable(const QStringList &apps, bool enable)
{
    // FileArmor不支持apps为空
    if (apps.isEmpty()) {
        cameraSetMode(enable ? PrivacySecurityDataProxy::AllDisabled : PrivacySecurityDataProxy::AllEnable);
    } else {
        QDBusMessage message = QDBusMessage::createMethodCall(CameraControlService, CameraControlPath, CameraControlInterface, "Enable");
        message << apps << enable;
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
        watcher->setProperty("DBusApps", apps);
        watcher->setProperty("DBusEnable", enable);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onCameraEnableFinished);
    }
}

void PrivacySecurityDataProxy::cameraGetApps()
{
    QDBusMessage message = QDBusMessage::createMethodCall(CameraControlService, CameraControlPath, CameraControlInterface, "GetApps");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onCameraGetAppsFinished);
}

void PrivacySecurityDataProxy::cameraSetMode(int mode)
{
    QDBusMessage message = QDBusMessage::createMethodCall(CameraControlService, CameraControlPath, CameraControlInterface, "SetMode");
    message << mode;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusMode", mode);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onCameraSetModeFinished);
}

void PrivacySecurityDataProxy::cameraGetMode()
{
    QDBusMessage message = QDBusMessage::createMethodCall(CameraControlService, CameraControlPath, CameraControlInterface, "GetMode");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onCameraGetModeFinished);
}

QMap<QString, QStringList> PrivacySecurityDataProxy::getCacheBlacklist()
{
    QMap<QString, QStringList> cacheBlacklist;
    QString data = m_dconfig->value("permissionBlacklist").toString();
    QJsonDocument doc = QJsonDocument::fromJson(data.toLatin1());
    if (doc.isObject()) {
        QJsonObject json = doc.object();
        for (auto &&key : json.keys()) {
            QJsonValue value = json.value(key);
            if (value.isArray()) {
                QJsonArray array = value.toArray();
                for (auto &&it = array.begin(); it != array.end(); it++) {
                    cacheBlacklist[key].append(it->toString());
                }
            }
        }
    }
    return cacheBlacklist;
}

void PrivacySecurityDataProxy::setCacheBlacklist(const QMap<QString, QStringList> &cacheBlacklist)
{
    QJsonObject json;
    for (auto &&it = cacheBlacklist.begin(); it != cacheBlacklist.end(); it++) {
        QJsonArray array;
        for (auto &&app : it.value()) {
            array.append(app);
        }
        json.insert(it.key(), array);
    }
    QJsonDocument doc(json);
    m_dconfig->setValue("permissionBlacklist", doc.toJson(QJsonDocument::Compact));
}

QMap<QString, QStringList> PrivacySecurityDataProxy::getPackagesExecutable(const QStringList &paths)
{
    QMap<QString, QStringList> packages;
    modstatdb_open(msdbrw_readonly);
    ensure_allinstfiles_available_quiet();
    ensure_diversions();
    struct fsys_namenode *namenode;
    for (auto &&path : paths) {
        namenode = fsys_hash_find_node(path.toLatin1().data(), FHFF_NOCOPY);
        struct fsys_node_pkgs_iter *iter = fsys_node_pkgs_iter_new(namenode);
        struct pkginfo *pkg_owner = fsys_node_pkgs_iter_next(iter);
        if (pkg_owner) {
            QStringList files;
            struct fsys_namenode_list *file;
            file = pkg_owner->files;
            while (file) {
                QFileInfo fileInfo(file->namenode->name);
                if (fileInfo.isFile() && fileInfo.isExecutable()) {
                    files << fileInfo.filePath();
                }
                file = file->next;
            }
            packages.insert(path, files);
        }
        fsys_node_pkgs_iter_free(iter);
    }
    modstatdb_shutdown();
    return packages;
}

void PrivacySecurityDataProxy::onGetItemInfosFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<AppItemInfoList> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT itemInfosChanged(reply.value());
    } else {
        qWarning() << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::onFileEnableFinished(QDBusPendingCallWatcher *w)
{
    // 设置Enable时会修改Mode
    QString file = w->property("DBusFile").toString();
    fileGetMode(file);
    fileGetApps(file);
    w->deleteLater();
}

void PrivacySecurityDataProxy::onFileListFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QStringList> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT fileListChanged(reply.value());
    } else {
        qWarning() << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::onFileGetAppsFinished(QDBusPendingCallWatcher *w)
{
    QString file = w->property("DBusFile").toString();

    QDBusMessage msg = w->reply();
    if (msg.type() != QDBusMessage::ErrorMessage) {
        Q_EMIT fileAppsChanged(file, QPair<QStringList, bool>(msg.arguments().at(0).toStringList(), msg.arguments().at(1).toBool()));
    } else {
        qWarning() << msg.errorName() << msg.errorMessage();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::onFileSetModeFinished(QDBusPendingCallWatcher *w)
{
    QString file = w->property("DBusFile").toString();
    int mode = w->property("DBusMode").toInt();
    QDBusPendingReply<> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT fileModeChanged(file, mode);
    } else {
        fileGetMode(file);
        qWarning() << reply.error();
    }
    fileGetApps(file);
    w->deleteLater();
}

void PrivacySecurityDataProxy::onFileGetModeFinished(QDBusPendingCallWatcher *w)
{
    QString file = w->property("DBusFile").toString();
    QDBusPendingReply<int> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT fileModeChanged(file, reply.value());
    } else {
        qWarning() << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::onCameraEnableFinished(QDBusPendingCallWatcher *w)
{
    // 设置Enable时会修改Mode
    cameraGetMode();
    cameraGetApps();
    QDBusMessage msg = w->reply();
    if (msg.type() == QDBusMessage::ErrorMessage) {
        qWarning() << msg.errorName() << msg.errorMessage();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::onCameraGetAppsFinished(QDBusPendingCallWatcher *w)
{
    QDBusMessage msg = w->reply();
    if (msg.type() != QDBusMessage::ErrorMessage) {
        Q_EMIT cameraAppsChanged(QPair<QStringList, bool>(msg.arguments().at(0).toStringList(), msg.arguments().at(1).toBool()));
    } else {
        qWarning() << msg.errorName() << msg.errorMessage();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::onCameraSetModeFinished(QDBusPendingCallWatcher *w)
{
    int mode = w->property("DBusMode").toInt();
    QDBusPendingReply<> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT cameraModeChanged(mode);
    } else {
        Q_EMIT cameraModeChanged(mode);
        cameraGetMode();
        qWarning() << reply.error();
    }
    cameraGetApps();
    w->deleteLater();
}

void PrivacySecurityDataProxy::onCameraGetModeFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<int> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT cameraModeChanged(reply.value());
    } else {
        qWarning() << reply.error();
    }
    w->deleteLater();
}
