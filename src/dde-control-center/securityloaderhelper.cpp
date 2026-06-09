// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "securityloaderhelper.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLoggingCategory>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <unistd.h>

Q_LOGGING_CATEGORY(secLoader, "org.deepin.dde.control-center.securityloader")

const QString SecurityLoaderHelper::DEFAULT_CONFIG_DIR = "/usr/share/dde-control-center/permission-interfaces";

SecurityLoaderHelper::SecurityLoaderHelper(QObject *parent)
    : QObject(parent)
{
}

SecurityLoaderHelper::~SecurityLoaderHelper() {}

SecurityLoaderHelper &SecurityLoaderHelper::instance()
{
    static SecurityLoaderHelper instance;
    return instance;
}

void SecurityLoaderHelper::doSecurityLoader(int fd1, int fd2)
{
    if (fd1 < 0 || fd2 < 0) {
        qCWarning(secLoader) << "Not loaded by loader, skipping handshake";
        return;
    }

    qCInfo(secLoader) << "Detected loader injection: fd1=" << fd1 << "fd2=" << fd2;

    loadConfigFromDirectory();
    // appendCurrentUserAccountsUserDest();
    if (!performHandshake(fd1, fd2)) {
        qCWarning(secLoader) << "Security loader handshake failed";
    }
}

void SecurityLoaderHelper::loadConfigFromDirectory(const QString &configDir)
{
    QString dir = configDir.isEmpty() ? DEFAULT_CONFIG_DIR : configDir;
    QDir directory(dir);

    if (!directory.exists()) {
        qCWarning(secLoader) << "Config directory does not exist:" << dir;
        return;
    }

    qCInfo(secLoader) << "Loading permission configs from:" << dir;

    m_destList = QJsonArray();
    QDirIterator it(dir, {"*.json"}, QDir::Files);

    while (it.hasNext()) {
        QString filePath = it.next();
        qCInfo(secLoader) << "Parsing config file:" << filePath;
        parseJsonFile(filePath);
    }

    qCInfo(secLoader) << "Loaded" << m_destList.size() << "D-Bus interfaces to authorize";
}

void SecurityLoaderHelper::parseJsonFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(secLoader) << "Cannot open file:" << filePath;
        return;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error != QJsonParseError::NoError) {
        qCWarning(secLoader) << "JSON parse error in" << filePath << ":" << error.errorString();
        return;
    }

    QJsonObject root = doc.object();
    if (!root.contains("DestList") || !root["DestList"].isArray()) {
        qCWarning(secLoader) << "Invalid config format in" << filePath << ": missing DestList";
        return;
    }

    for (const auto &item : root["DestList"].toArray()) {
        appendDest(item.toObject());
    }
}

void SecurityLoaderHelper::appendDest(const QJsonObject &dest)
{
    const QString dbusName = dest["DbusName"].toString();
    const QString dbusPath = dest["DbusPath"].toString();
    const QString dbusInterface = dest["DbusInterface"].toString();

    if (dbusName.isEmpty() || dbusPath.isEmpty() || dbusInterface.isEmpty()) {
        qCWarning(secLoader) << "Skip invalid D-Bus destination:" << dest;
        return;
    }

    for (const auto &existing : m_destList) {
        const QJsonObject current = existing.toObject();
        if (current["DbusName"] == dbusName &&
            current["DbusPath"] == dbusPath &&
            current["DbusInterface"] == dbusInterface) {
            return;
        }
    }

    m_destList.append(dest);
    qCInfo(secLoader) << "  Added:" << dbusName << dbusPath << dbusInterface;
}

void SecurityLoaderHelper::appendCurrentUserAccountsUserDest()
{
    const QString userPath = currentUserAccountsPath();
    if (userPath.isEmpty()) {
        qCWarning(secLoader) << "Cannot resolve current user's Accounts.User path";
        return;
    }

    QJsonObject dest;
    dest["DbusName"] = QStringLiteral("org.deepin.dde.Accounts1");
    dest["DbusPath"] = userPath;
    dest["DbusInterface"] = QStringLiteral("org.deepin.dde.Accounts1.User");
    appendDest(dest);
}

QString SecurityLoaderHelper::currentUserAccountsPath() const
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(secLoader) << "Cannot connect to system bus when resolving current user path";
        return {};
    }

    QDBusInterface accountsInterface(QStringLiteral("org.deepin.dde.Accounts1"),
                                     QStringLiteral("/org/deepin/dde/Accounts1"),
                                     QStringLiteral("org.deepin.dde.Accounts1"),
                                     systemBus);
    if (!accountsInterface.isValid()) {
        qCWarning(secLoader) << "Accounts interface invalid when resolving current user path:"
                             << accountsInterface.lastError().message();
        return {};
    }

    QDBusReply<QString> reply = accountsInterface.call(QStringLiteral("FindUserById"), QString::number(getuid()));
    if (!reply.isValid()) {
        qCWarning(secLoader) << "FindUserById failed when resolving current user path:"
                             << reply.error().message();
        return {};
    }

    return reply.value();
}

bool SecurityLoaderHelper::performHandshake(int fd1, int fd2)
{
    if (fd1 < 0 || fd2 < 0) {
        qCInfo(secLoader) << "Invalid fd, skipping handshake";
        return true;
    }

    if (m_destList.isEmpty()) {
        qCInfo(secLoader) << "No D-Bus interfaces loaded, skipping handshake";
        return true;
    }

    qCInfo(secLoader) << "Performing loader handshake...";

    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(secLoader) << "Cannot connect to system bus";
        return false;
    }
    QString uniqueName = systemBus.baseService();
    qCInfo(secLoader) << "System Bus UniqueName:" << uniqueName;

    QJsonObject request;
    request["UniqueName"] = uniqueName;
    request["DestList"] = m_destList;

    QJsonDocument doc(request);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    qCInfo(secLoader) << "Send request with" << m_destList.size() << "interfaces";

    QFile fd1File;
    if (!fd1File.open(fd1, QIODevice::WriteOnly)) {
        qCWarning(secLoader) << "Cannot open fd1 for writing";
        return false;
    }
    fd1File.write(jsonData);
    fd1File.close();

    QFile fd2File;
    if (!fd2File.open(fd2, QIODevice::ReadOnly)) {
        qCWarning(secLoader) << "Cannot open fd2 for reading";
        return false;
    }

    QByteArray response = fd2File.readAll();
    fd2File.close();

    QJsonParseError parseError;
    QJsonDocument responseDoc = QJsonDocument::fromJson(response, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qCWarning(secLoader) << "Invalid JSON response from loader:" << parseError.errorString();
        return false;
    }

    QJsonObject result = responseDoc.object();
    if (result["Result"].toBool()) {
        qCInfo(secLoader) << "Loader handshake completed successfully";
        return true;
    } else {
        qCWarning(secLoader) << "Loader authorization response:" << result["Message"].toString();
        return false;
    }
}
