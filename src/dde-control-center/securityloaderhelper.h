// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SECURITYLOADERHELPER_H
#define SECURITYLOADERHELPER_H

#include <QObject>
#include <QJsonArray>

class SecurityLoaderHelper : public QObject
{
    Q_OBJECT

public:
    static SecurityLoaderHelper &instance();
    void doSecurityLoader(int fd1, int fd2);

private:
    explicit SecurityLoaderHelper(QObject *parent = nullptr);
    ~SecurityLoaderHelper();

    SecurityLoaderHelper(const SecurityLoaderHelper &) = delete;
    SecurityLoaderHelper &operator=(const SecurityLoaderHelper &) = delete;

    void loadConfigFromDirectory(const QString &configDir = QString());
    void parseJsonFile(const QString &filePath);
    void appendDest(const QJsonObject &dest);
    void appendCurrentUserAccountsUserDest();
    QString currentUserAccountsPath() const;
    bool performHandshake(int fd1, int fd2);

private:
    QJsonArray m_destList;
    static const QString DEFAULT_CONFIG_DIR;
};

#endif // SECURITYLOADERHELPER_H
