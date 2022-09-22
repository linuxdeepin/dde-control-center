// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "secretagent.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>

#include <QDBusConnection>
#include <QStringBuilder>
#include <QtDBus/qdbusmetatype.h>
#include <QJsonDocument>
#include <QJsonArray>

static const QString NetworkDialogApp = "dde-network-dialog"; //网络列表执行文件

#define DEBUG_PRINT qDebug() << __FILE__ << "line:" << __LINE__ << "function:" << __FUNCTION__ << "Message:"

NETWORKPLUGIN_USE_NAMESPACE

SecretAgent::SecretAgent(bool greeter, QObject *parent)
    : NetworkManager::SecretAgent(QStringLiteral("com.deepin.system.network.SecretAgent"), parent)
{
    m_greeter = greeter;
    qInfo() << "register secret agent: com.deepin.system.network.SecretAgent mode: " << (m_greeter ? "greeter" : "lock");

    qDBusRegisterMetaType<NMVariantMapMap>();
}

SecretAgent::~SecretAgent() = default;

NMVariantMapMap SecretAgent::GetSecrets(const NMVariantMapMap &connection,
                                        const QDBusObjectPath &connection_path,
                                        const QString &setting_name,
                                        const QStringList &hints,
                                        uint flags)
{
    DEBUG_PRINT << "Path:" << connection_path.path();
    DEBUG_PRINT << "Setting name:" << setting_name;
    DEBUG_PRINT << "Hints:" << hints;
    DEBUG_PRINT << "Flags:" << flags;

    const QString callId = connection_path.path() % setting_name;
    for (const SecretsRequest &request : m_calls) {
        if (request == callId) {
            qWarning() << "GetSecrets was called again! This should not happen, cancelling first call" << connection_path.path()
                                          << setting_name;
            CancelGetSecrets(connection_path, setting_name);
            break;
        }
    }

    setDelayedReply(true);
    SecretsRequest request(SecretsRequest::GetSecrets);
    request.callId = callId;
    request.connection = connection;
    request.connection_path = connection_path;
    request.flags = static_cast<NetworkManager::SecretAgent::GetSecretsFlags>(flags);
    request.hints = hints;
    request.setting_name = setting_name;
    request.message = message();
    m_calls << request;

    processNext();

    return {};
}

void SecretAgent::SaveSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path)
{
    setDelayedReply(true);
    SecretsRequest::Type type;
    if (hasSecrets(connection)) {
        type = SecretsRequest::SaveSecrets;
    } else {
        type = SecretsRequest::DeleteSecrets;
    }
    SecretsRequest request(type);
    request.connection = connection;
    request.connection_path = connection_path;
    request.message = message();
    m_calls << request;

    processNext();
}

void SecretAgent::DeleteSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path)
{
    setDelayedReply(true);
    SecretsRequest request(SecretsRequest::DeleteSecrets);
    request.connection = connection;
    request.connection_path = connection_path;
    request.message = message();
    m_calls << request;

    processNext();
}

void SecretAgent::CancelGetSecrets(const QDBusObjectPath &connection_path, const QString &setting_name)
{
    QString callId = connection_path.path() % setting_name;
    for (int i = 0; i < m_calls.size(); ++i) {
        SecretsRequest request = m_calls.at(i);
        if (request.type == SecretsRequest::GetSecrets && callId == request.callId) {
            if (m_process == request.process) {
                DEBUG_PRINT << "process finished (agent canceled)";
                m_process->deleteLater();
                m_process = nullptr;
            }
            sendError(SecretAgent::AgentCanceled, QStringLiteral("Agent canceled the password dialog"), request.message);
            m_calls.removeAt(i);
            break;
        }
    }

    processNext();
}

void SecretAgent::processNext()
{
    int i = 0;
    while (i < m_calls.size()) {
        SecretsRequest &request = m_calls[i];
        switch (request.type) {
        case SecretsRequest::GetSecrets:
            if (processGetSecrets(request)) {
                m_calls.removeAt(i);
                continue;
            }
            break;
        case SecretsRequest::SaveSecrets:
            if (processSaveSecrets(request)) {
                m_calls.removeAt(i);
                continue;
            }
            break;
        case SecretsRequest::DeleteSecrets:
            if (processDeleteSecrets(request)) {
                m_calls.removeAt(i);
                continue;
            }
            break;
        }
        ++i;
    }
}

void SecretAgent::dialogFinished()
{
    if (!m_process) return;

    readProcessOutput();

    for (int i = 0; i < m_calls.size(); ++i) {
        SecretsRequest request = m_calls[i];
        if (request.type == SecretsRequest::GetSecrets && request.process == m_process) {
            m_calls.removeAt(i);
            break;
        }
    }
    DEBUG_PRINT << "process finished";
    m_process->deleteLater();
    m_process = nullptr;

    processNext();
}

void SecretAgent::readProcessOutput()
{
    for (int i = 0; i < m_calls.size(); ++i) {
        SecretsRequest request = m_calls[i];
        if (request.type == SecretsRequest::GetSecrets && request.process == m_process) {
            QByteArray outData;
            QByteArray allData = m_process->readAllStandardOutput();

            allData = m_lastData + allData;
            QList<QByteArray> dataArray = allData.split('\n');
            m_lastData = dataArray.last();
            for (const QByteArray &data : dataArray) {
                if (data.startsWith("password:")) {
                    QByteArray cmd = data.mid(9);
                    QJsonDocument doc = QJsonDocument::fromJson(cmd);
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        QString key = obj.value("key").toString();
                        QString passwd = obj.value("password").toString();
                        bool input = obj.value("input").toBool();
                        DEBUG_PRINT << "key: " << key;
                        DEBUG_PRINT << "password: " << passwd;
                        DEBUG_PRINT << "input: " << input;
                        if (input) {
                            QJsonObject resultJsonObj;
                            QJsonArray secretsJsonArray;
                            secretsJsonArray.append(passwd);
                            resultJsonObj.insert("secrets", secretsJsonArray);

                            NetworkManager::ConnectionSettings::Ptr connectionSettings =
                                            NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(request.connection));
                            NetworkManager::Setting::Ptr setting = connectionSettings->setting(request.setting_name);
                            auto needSecrets = setting->needSecrets(request.flags & RequestNew);
                            if (!passwd.isEmpty() && !needSecrets.isEmpty()) {
                                QVariantMap result;
                                result.insert(needSecrets.first(), passwd);
                                request.connection[request.setting_name] = result;
                                sendSecrets(request.connection, request.message);
                            }
                        } else {
                            sendError(SecretAgent::UserCanceled, QStringLiteral("user canceled"), request.message);
                        }

                        break;
                    }
                }
            }
        }
    }
}

bool SecretAgent::processGetSecrets(SecretsRequest &request) const
{
    if (m_process) {
        return false;
    }

    NetworkManager::ConnectionSettings::Ptr connectionSettings =
        NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(request.connection));
    NetworkManager::Setting::Ptr setting = connectionSettings->setting(request.setting_name);

    const bool requestNew = request.flags & RequestNew;
    const bool userRequested = request.flags & UserRequested;
    const bool allowInteraction = request.flags & AllowInteraction;
    const bool isVpn = (setting->type() == NetworkManager::Setting::Vpn);

    if (isVpn) {
        NetworkManager::VpnSetting::Ptr vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
        if (vpnSetting->serviceType() == QLatin1String("org.freedesktop.NetworkManager.ssh") && vpnSetting->data()["auth-type"] == QLatin1String("ssh-agent")) {
            QString authSock = qgetenv("SSH_AUTH_SOCK");
            qDebug() << Q_FUNC_INFO << "Sending SSH auth socket" << authSock;

            if (authSock.isEmpty()) {
                sendError(SecretAgent::NoSecrets, QStringLiteral("SSH_AUTH_SOCK not present"), request.message);
            } else {
                NMStringMap secrets;
                secrets.insert(QStringLiteral("ssh-auth-sock"), authSock);

                QVariantMap secretData;
                secretData.insert(QStringLiteral("secrets"), QVariant::fromValue<NMStringMap>(secrets));
                request.connection[request.setting_name] = secretData;
                sendSecrets(request.connection, request.message);
            }
            return true;
        }
    }

    if (!m_greeter) {
        NMStringMap secretsMap;
        if (!requestNew) {
            // 需要去解锁密码环，取密码
            return false;
        }

        if (!secretsMap.isEmpty()) {
            setting->secretsFromStringMap(secretsMap);
            if (!(isVpn) && setting->needSecrets(requestNew).isEmpty()) {
                // Enough secrets were retrieved from storage
                request.connection[request.setting_name] = setting->secretsToMap();
                sendSecrets(request.connection, request.message);
                return true;
            }
        }
    }

    if (requestNew || (allowInteraction && !setting->needSecrets(requestNew).isEmpty()) || (allowInteraction && userRequested)
               || (isVpn && allowInteraction)) {
        DEBUG_PRINT << "process request secrets";

        // 只处理无线的
        if (connectionSettings->connectionType() != NetworkManager::ConnectionSettings::Wireless) {
            sendError(SecretAgent::InternalError, QStringLiteral("dss can only handle wireless"), request.message);
            return true;
        }

        if (!userRequested) {
            sendError(SecretAgent::AgentCanceled, QStringLiteral("dss only support user quest"), request.message);
            return true;
        }

        m_process = new QProcess;

        connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &SecretAgent::dialogFinished);
        connect(m_process, &QProcess::readyReadStandardOutput, this, &SecretAgent::readProcessOutput);

        QStringList argList;
        argList << "-w" << "-c" << connectionSettings->id();
        m_process->start(NetworkDialogApp, argList);
        m_process->waitForStarted();

        if (m_process->state() == QProcess::Running) {
            DEBUG_PRINT << "process begin";
            request.process = m_process;
            return false;
        } else {
            DEBUG_PRINT << "process can't running: " << NetworkDialogApp;
            sendError(SecretAgent::InternalError, QStringLiteral("dss start process failed"), request.message);
            delete m_process;
            m_process = nullptr;
            return true;
        }
    } else if (isVpn && userRequested) { // just return what we have
        NMVariantMapMap result;
        NetworkManager::VpnSetting::Ptr vpnSetting;
        vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
        // FIXME workaround when NM is asking for secrets which should be system-stored, if we send an empty map it
        // won't ask for additional secrets with AllowInteraction flag which would display the authentication dialog
        if (vpnSetting->secretsToMap().isEmpty()) {
            // Insert an empty secrets map as it was before I fixed it in NetworkManagerQt to make sure NM will ask again
            // with flags we need
            QVariantMap secretsMap;
            secretsMap.insert(QStringLiteral("secrets"), QVariant::fromValue<NMStringMap>(NMStringMap()));
            result.insert(QStringLiteral("vpn"), secretsMap);
        } else {
            result.insert(QStringLiteral("vpn"), vpnSetting->secretsToMap());
        }
        sendSecrets(result, request.message);
        return true;
    } else if (setting->needSecrets().isEmpty()) {
        NMVariantMapMap result;
        result.insert(setting->name(), setting->secretsToMap());
        sendSecrets(result, request.message);
        return true;
    } else {
        sendError(SecretAgent::InternalError, QStringLiteral("dss did not know how to handle the request"), request.message);
        return true;
    }
}

bool SecretAgent::processSaveSecrets(SecretsRequest &request) const
{
    if (!request.saveSecretsWithoutReply) {
        QDBusMessage reply = request.message.createReply();
        if (!QDBusConnection::systemBus().send(reply)) {
            qWarning() << "Failed put save secrets reply into the queue";
        }
    }

    return true;
}

bool SecretAgent::processDeleteSecrets(SecretsRequest &request) const
{
    QDBusMessage reply = request.message.createReply();
    if (!QDBusConnection::systemBus().send(reply)) {
        qWarning() << "Failed put delete secrets reply into the queue";
    }

    return true;
}

bool SecretAgent::hasSecrets(const NMVariantMapMap &connection) const
{
    NetworkManager::ConnectionSettings connectionSettings(connection);
    for (const NetworkManager::Setting::Ptr &setting : connectionSettings.settings()) {
        if (!setting->secretsToMap().isEmpty()) {
            return true;
        }
    }

    return false;
}

void SecretAgent::sendSecrets(const NMVariantMapMap &secrets, const QDBusMessage &message) const
{
    QDBusMessage reply;
    reply = message.createReply(QVariant::fromValue(secrets));
    if (!QDBusConnection::systemBus().send(reply)) {
        qWarning() << "Failed put the secret into the queue";
    }
}

