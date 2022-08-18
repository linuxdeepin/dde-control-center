/*
 * Copyright (C) 2011 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     liaohanqin <liaohanqin@uniontech.com>
 *
 * Maintainer: liaohanqin <liaohanqin@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SECRETAGENT_H
#define SECRETAGENT_H

#include "utils.h"

#include <NetworkManagerQt/SecretAgent>
#include <QProcess>

class QProcess;

NETWORKPLUGIN_BEGIN_NAMESPACE

class SecretsRequest
{
public:
    enum Type {
        GetSecrets,
        SaveSecrets,
        DeleteSecrets,
    };
    explicit SecretsRequest(Type _type)
        : type(_type)
        , flags(NetworkManager::SecretAgent::None)
        , saveSecretsWithoutReply(false)
    {
    }
    inline bool operator==(const QString &other) const
    {
        return callId == other;
    }
    Type type;
    QString callId;
    NMVariantMapMap connection;
    QDBusObjectPath connection_path;
    QString setting_name;
    QStringList hints;
    NetworkManager::SecretAgent::GetSecretsFlags flags;
    /**
     * When a user connection is called on GetSecrets,
     * the secret agent is supposed to save the secrets
     * typed by user, when true proccessSaveSecrets
     * should skip the DBus reply.
     */
    bool saveSecretsWithoutReply;
    QDBusMessage message;
    QProcess *process = nullptr;
};

class Q_DECL_EXPORT SecretAgent : public NetworkManager::SecretAgent
{
    Q_OBJECT
public:
    explicit SecretAgent(bool greeter = false, QObject *parent = nullptr);

    ~SecretAgent() override;

Q_SIGNALS:
    void secretsError(const QString &connectionPath, const QString &message) const;

public Q_SLOTS:
    NMVariantMapMap GetSecrets(const NMVariantMapMap &, const QDBusObjectPath &, const QString &, const QStringList &, uint) override;
    void SaveSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path) override;
    void DeleteSecrets(const NMVariantMapMap &, const QDBusObjectPath &) override;
    void CancelGetSecrets(const QDBusObjectPath &, const QString &) override;

private Q_SLOTS:
    void dialogFinished();
    void readProcessOutput();

private:
    void processNext();
    /**
     * @brief processGetSecrets requests
     * @param request the request we are processing
     * @param ignoreWallet true if the code should avoid Wallet
     * normally if it failed to open
     * @return true if the item was processed
     */
    bool processGetSecrets(SecretsRequest &request) const;
    bool processSaveSecrets(SecretsRequest &request) const;
    bool processDeleteSecrets(SecretsRequest &request) const;

    /**
     * @brief hasSecrets verifies if the desired connection has secrets to store
     * @param connection map with or without secrets
     * @return true if the connection has secrets, false otherwise
     */
    bool hasSecrets(const NMVariantMapMap &connection) const;
    void sendSecrets(const NMVariantMapMap &secrets, const QDBusMessage &message) const;

    mutable QProcess *m_process = nullptr;
    QList<SecretsRequest> m_calls;
    QByteArray m_lastData; // 用于数据拼接
    bool m_greeter;
};

NETWORKPLUGIN_END_NAMESPACE
#endif // SECRETAGENT_H
