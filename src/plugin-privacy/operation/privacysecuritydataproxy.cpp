// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "privacysecuritydataproxy.h"

#include <DConfig>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLibrary>
#include <QStandardPaths>
#include <QProcess>

#define DBUS_TIMEOUT 10000

const QString UsecService = "org.deepin.usec1";
const QString UsecPath = "/org/deepin/usec1/AccessControl";
const QString UsecInterface = "org.deepin.usec1.AccessControl";

static QString getDpkgArch()
{
    const static QString arch = []() {
        QString arch = QSysInfo::currentCpuArchitecture();
        QProcess pro;
        pro.start("/usr/bin/dpkg", QStringList() << "--print-architecture");
        pro.waitForFinished(2000);
        if (pro.exitCode() != 0) {
            qCWarning(DCC_PRIVACY) << "Failed to get dpkg architecture, dpkg error: " << pro.readAllStandardError().simplified() 
                << ", fallback to QSysInfo architecture:" << arch;
            return arch;
        }
        QString output = pro.readAllStandardOutput().simplified();
        if (output.isEmpty()) {
            qCWarning(DCC_PRIVACY) << "No architecture found for dpkg, fallback to QSysInfo architecture:" << arch;
            return arch;
        }
        return output;
    }();
    return arch;
}

PrivacySecurityDataProxy::PrivacySecurityDataProxy(QObject *parent)
    : QObject(parent)
    , m_serviceExists(false)
    , m_dconfig(nullptr)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "GetNameOwner");
    message << UsecService;
    QDBusConnection::systemBus().callWithCallback(message, this, SLOT(onGetNameOwner(QString)));
    QDBusConnection::systemBus().connect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged", this, SLOT(onDBusNameOwnerChanged(QString, QString, QString)));
}

PrivacySecurityDataProxy::~PrivacySecurityDataProxy()
{
}

void PrivacySecurityDataProxy::getMode(const QString &object)
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "GetMode");
    message << object;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusObject", object);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onGetModeFinished);
}

void PrivacySecurityDataProxy::onGetModeFinished(QDBusPendingCallWatcher *w)
{
    QString object = w->property("DBusObject").toString();
    QDBusPendingReply<QString> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT ModeChanged(reply.value(), "modify");
    } else {
        qCWarning(DCC_PRIVACY) << "Get " << object << " mode failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::setMode(const QString &config)
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "SetMode");
    message << config;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusConfig", config);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onSetModeFinished);
}

void PrivacySecurityDataProxy::onSetModeFinished(QDBusPendingCallWatcher *w)
{
    QString config = w->property("DBusConfig").toString();
    QDBusPendingReply<> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT ModeChanged(config, "modify");
    } else {
        qCWarning(DCC_PRIVACY) << "Set " << config << " mode failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::listEntity()
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "ListEntity");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onListEntityFinished);
}

void PrivacySecurityDataProxy::onListEntityFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QStringList> reply = w->reply();
    if (!reply.isError()) {
        for (auto &&entity : reply.value()) {
            Q_EMIT EntityChanged(entity, "modify");
        }
    } else {
        qCWarning(DCC_PRIVACY) << "Get entity list failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::getEntity(const QString &entity)
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "GetEntity");
    message << entity;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusEntity", entity);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onGetEntityFinished);
}

void PrivacySecurityDataProxy::onGetEntityFinished(QDBusPendingCallWatcher *w)
{
    QString entity = w->property("DBusEntity").toString();
    QDBusPendingReply<QString> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT EntityChanged(reply.value(), "modify");
    } else {
        qCWarning(DCC_PRIVACY) << "Get" << entity << "entity failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::setEntity(const QString &entity)
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "SetEntity");
    message << entity;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusEntity", entity);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onSetEntityFinished);
}

void PrivacySecurityDataProxy::onSetEntityFinished(QDBusPendingCallWatcher *w)
{
    QString entity = w->property("DBusEntity").toString();
    QDBusPendingReply<> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT EntityChanged(entity, "modify");
    } else {
        qCWarning(DCC_PRIVACY) << "Set" << entity << "entity failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::getPolicy(const QString &entity)
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "GetPolicy");
    message << entity;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusEntity", entity);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onGetPolicyFinished);
}

void PrivacySecurityDataProxy::onGetPolicyFinished(QDBusPendingCallWatcher *w)
{
    QString entity = w->property("DBusEntity").toString();
    QDBusPendingReply<QString> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT PolicyChanged(reply.value(), "modify");
    } else {
        qCWarning(DCC_PRIVACY) << "Get" << entity << "policy failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::setPolicy(const QString &policy)
{
    QDBusMessage message = QDBusMessage::createMethodCall(UsecService, UsecPath, UsecInterface, "SetPolicy");
    message << policy;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, DBUS_TIMEOUT), this);
    watcher->setProperty("DBusPolicy", policy);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &PrivacySecurityDataProxy::onSetPolicyFinished);
}

void PrivacySecurityDataProxy::onSetPolicyFinished(QDBusPendingCallWatcher *w)
{
    QString policy = w->property("DBusPolicy").toString();
    QDBusPendingReply<> reply = w->reply();
    if (!reply.isError()) {
        Q_EMIT PolicyChanged(policy, "modify");
    } else {
        qCWarning(DCC_PRIVACY) << "Set" << policy << "policy failed, DBus reply error: " << reply.error();
    }
    w->deleteLater();
}

void PrivacySecurityDataProxy::init()
{
    m_dconfig = Dtk::Core::DConfig::create("org.deepin.dde.control-center", "org.deepin.dde.control-center.privacy");

    QDBusConnection::systemBus().connect(UsecService, UsecPath, UsecInterface, "PolicyChanged", this, SIGNAL(PolicyChanged(QString, QString)));
    QDBusConnection::systemBus().connect(UsecService, UsecPath, UsecInterface, "EntityChanged", this, SIGNAL(EntityChanged(QString, QString)));
    QDBusConnection::systemBus().connect(UsecService, UsecPath, UsecInterface, "ModeChanged", this, SIGNAL(ModeChanged(QString, QString)));
}

bool PrivacySecurityDataProxy::existsService() const
{
    return m_serviceExists;
}

QMap<QString, QSet<QString>> PrivacySecurityDataProxy::getCacheBlacklist()
{
    QMap<QString, QSet<QString>> cacheBlacklist;
    QString data = m_dconfig->value("permissionBlacklist").toString();
    QJsonDocument doc = QJsonDocument::fromJson(data.toLatin1());
    if (doc.isObject()) {
        QJsonObject json = doc.object();
        for (auto &&key : json.keys()) {
            QJsonValue value = json.value(key);
            if (value.isArray()) {
                QJsonArray array = value.toArray();
                for (auto &&it = array.begin(); it != array.end(); it++) {
                    cacheBlacklist[key].insert(it->toString());
                }
            }
        }
    }
    return cacheBlacklist;
}

void PrivacySecurityDataProxy::setCacheBlacklist(const QMap<QString, QSet<QString>> &cacheBlacklist)
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

QStringList PrivacySecurityDataProxy::getExecutable(const QString &path, QString *package)
{
    QProcess pro;
    QStringList args;
    args << "-S" << path;
    pro.start("/usr/bin/dpkg-query", args);
    pro.waitForFinished(2000);
    
    if (pro.exitCode() != 0) {
        qCWarning(DCC_PRIVACY) << "Failed to get executable for" << path << ", dpkg-query error: " << pro.readAllStandardError().simplified();
        return {};
    }

    QString output = pro.readAllStandardOutput().simplified();
    if (output.isEmpty()) {
        qCWarning(DCC_PRIVACY) << "No package found for executable:" << path;
        return {};
    }

    *package = output.split('\n').first().split(':').first();
    QString listFilePath = QString("/var/lib/dpkg/info/%1.list").arg(*package);
    if (!QFile::exists(listFilePath)) {
        listFilePath = QString("/var/lib/dpkg/info/%1.list").arg(*package + ":" + getDpkgArch());
    }
    QFile listFile(listFilePath);

    if (!listFile.exists() || !listFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(DCC_PRIVACY) << "Failed to open list file:" << listFilePath;
        return {};
    }

    QStringList files;
    QTextStream stream(&listFile);
    while (!stream.atEnd()) {
        QString filePath = stream.readLine().trimmed();
        if (filePath.isEmpty()) {
            continue;
        }

        QFileInfo fileInfo(filePath);
        if (fileInfo.exists() && 
            fileInfo.isFile() && 
            fileInfo.isExecutable() && 
            !QLibrary::isLibrary(filePath)) {
            files << filePath;
        }
    }

    listFile.close();
    return files;
}

void PrivacySecurityDataProxy::onGetNameOwner(const QString &)
{
    updateServiceExists(true);
}

void PrivacySecurityDataProxy::onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    if (UsecService == name) {
        if (newOwner.isEmpty()) { // remove
            updateServiceExists(false);
        } else if (oldOwner.isEmpty()) { // add
            updateServiceExists(true);
        }
    }
}

void PrivacySecurityDataProxy::updateServiceExists(bool serviceExists)
{
    if (serviceExists != m_serviceExists) {
        m_serviceExists = serviceExists;
        Q_EMIT serviceExistsChanged(m_serviceExists);
    }
}
