// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPINIDMODEL_H
#define DEEPINIDMODEL_H

#include "syncinfolistmodel.h"
#include "appinfolistmodel.h"
#include "downloadurl.h"

#include <QObject>
#include <QVariantMap>

class DeepinidModel : public QObject
{
    Q_OBJECT

    // deepinid
    Q_PROPERTY(bool loginState READ getLoginState NOTIFY loginStateChanged)
    Q_PROPERTY(QString avatar READ getAvatar NOTIFY avatarChanged)
    Q_PROPERTY(QString region READ getRegion NOTIFY regionChanged)
    Q_PROPERTY(QString userName READ getUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString wechatName READ getWechatName NOTIFY wechatNameChanged)

    // 同步服务
    Q_PROPERTY(bool syncEnabled READ syncEnabled NOTIFY syncEnabledChanged)
    Q_PROPERTY(bool syncSwitch READ syncSwitch WRITE setSyncSwitch NOTIFY syncSwitchChanged)
    Q_PROPERTY(bool syncItemShow READ syncItemShow WRITE setSyncItemShow NOTIFY syncItemShowChanged)
    Q_PROPERTY(QString lastSyncTime READ lastSyncTime NOTIFY lastSyncTimeChanged)

public:
    explicit DeepinidModel(QObject *parent = nullptr);

    // deepinid账户
    inline const QVariantMap userinfo() const { return m_userinfo; }
    void setUserinfo(const QVariantMap &userinfo);

    inline bool getLoginState() const { return m_loginState; }
    inline const QString getAvatar() const { return m_avatar; }
    inline const QString getRegion() const { return m_region; }
    inline const QString getUserName() const { return m_userName; }
    inline const QString getWechatName() const { return m_wechatName; }

    // 同步服务
    inline bool activation() const { return m_activation; }
    void setActivation(bool activation);

    bool syncEnabled() const;

    inline bool syncSwitch() const { return m_syncSwitch; }
    void setSyncSwitch(bool enable);

    inline bool syncItemShow() const { return m_syncItemShow; }
    void setSyncItemShow(bool show);

    inline QString lastSyncTime() const { return m_lastSyncTime; }
    void setLastSyncTime(const qlonglong &lastSyncTime);

    void updateSyncItem(const QString &key, bool enable);

    void initAppItemList(QList<AppItemData*> appItemList);
    void updateAppItem(const QString &key, bool enable);

    Q_INVOKABLE SyncInfoListModel* syncInfoListModel() const { return m_syncInfoListModel; }
    Q_INVOKABLE AppInfoListModel* appInfoListModel() const { return m_appInfoListModel; }

    Q_INVOKABLE QString warnTipsMessage(); // 获取警告提示信息

protected Q_SLOTS:
    void updateAvatarPath();

signals:
    void loginStateChanged(bool loginState);
    void avatarChanged(const QString &avatar);
    void regionChanged(const QString &region);
    void userNameChanged(const QString &userName);
    void wechatNameChanged(const QString &wechatName);

    void syncEnabledChanged(bool enable);
    void syncSwitchChanged(bool enable);
    void syncItemShowChanged(bool show);
    void syncStateChanged(const std::pair<qint32, QString>& syncState);
    void lastSyncTimeChanged(const QString &lastSyncTime);

private:
    // deepinid账户
    QVariantMap m_userinfo;
    bool m_loginState;
    QString m_avatar;
    QString m_region;
    QString m_userName;
    QString m_wechatName;

    // 同步服务
    bool m_activation;   // 激活状态
    bool m_syncEnabled;  // 同步是否可用：受激活状态、地区限制(未激活不可用，非中国地区不可用)
    bool m_syncSwitch;   // 同步开关是否已开启
    bool m_syncItemShow; // 是否显示系统设置中的同步项
    qlonglong m_syncTimestamp; // 上次同步时间戳
    QString m_lastSyncTime;    // 上次同步时间
    SyncInfoListModel *m_syncInfoListModel;
    AppInfoListModel *m_appInfoListModel;
    DownloadUrl *m_downloader;
};

#endif // DEEPINIDMODEL_H
