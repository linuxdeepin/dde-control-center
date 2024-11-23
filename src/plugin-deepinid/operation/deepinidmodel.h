// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPINIDMODEL_H
#define DEEPINIDMODEL_H

#include <QObject>
#include <QVariantMap>

class DeepinidModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loginState READ getLoginState NOTIFY loginStateChanged)
    Q_PROPERTY(QString avatar READ getAvatar NOTIFY avatarChanged)
    Q_PROPERTY(QString region READ getRegion NOTIFY regionChanged)
    Q_PROPERTY(QString displayName READ getDisplayName NOTIFY displayNameChanged)

    Q_PROPERTY(bool activation READ activation WRITE setActivation NOTIFY activationChanged)
    Q_PROPERTY(bool enableSync READ enableSync WRITE setEnableSync NOTIFY enableSyncChanged)


public:
    explicit DeepinidModel(QObject *parent = nullptr);

    inline const QVariantMap userinfo() const { return m_userinfo; }
    void setUserinfo(const QVariantMap &userinfo);

    inline bool getLoginState() const { return m_loginState; }
    inline const QString getAvatar() const { return m_avatar; }
    inline const QString getRegion() const { return m_region; }
    inline const QString getDisplayName() const { return m_displayName; }

    inline bool activation() const { return m_activation; }
    void setActivation(bool activation);

    inline bool enableSync() const { return m_enableSync; }
    void setEnableSync(bool enableSync);

private:
    void updateAvatarPath(const QString &avatar);

signals:
    void loginStateChanged(bool loginState);
    void avatarChanged(const QString &avatar);
    void regionChanged(const QString &region);
    void displayNameChanged(const QString &displayName);

    void enableSyncChanged(bool enableSync);
    void activationChanged(bool activation);

private:
    QVariantMap m_userinfo;
    bool m_loginState;
    QString m_avatar;
    QString m_region;
    QString m_displayName;

    bool m_activation;
    bool m_enableSync;
    

};

#endif // DEEPINIDMODEL_H
