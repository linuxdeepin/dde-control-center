//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QDBusVariant>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace DCC_NAMESPACE{

class NotificationSetting;

class AppItemModel : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        APPNAME,
        APPICON,
        ENABELNOTIFICATION,
        ENABELPREVIEW,
        ENABELSOUND,
        SHOWINNOTIFICATIONDESKTOP,
        SHOWINNOTIFICATIONCENTER,
        LOCKSCREENSHOWNOTIFICATION
    } AppConfigurationItem;

    explicit AppItemModel(NotificationSetting *setting, QObject *parent = nullptr);

    Q_PROPERTY(QString softName READ softName WRITE setSoftName NOTIFY softNameChanged FINAL)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged FINAL)
    inline QString softName()const {return  m_softName;}
    void setSoftName(const QString &name);

    inline QString icon()const {return m_icon;}
    void setIcon(const QString &icon);

    inline bool isAllowNotify()const {return m_isAllowNotify;}
    void setAllowNotify(const bool &state);

    inline bool isNotifySound()const {return m_isNotifySound;}
    void setNotifySound(const bool &state);

    inline bool isLockShowNotify()const {return m_isLockShowNotify;}
    void setLockShowNotify(const bool &state);

    inline bool isShowDesktop()const {return m_isShowDesktop;}
    void setShowDesktop(const bool &state);

    inline bool isShowInNotifyCenter()const {return m_isShowInNotifyCenter;}
    void setShowInNotifyCenter(const bool &state);

    inline bool isShowNotifyPreview()const {return m_isShowNotifyPreview;}
    void setShowNotifyPreview(const bool &state);

    inline QString getActName()const {return m_actName;}
    void setActName(const QString &name);

public Q_SLOTS:
    void onSettingChanged(const QString &id, const uint &item, QDBusVariant var);

Q_SIGNALS:
    void softNameChanged(QString name);
    void iconChanged(QString icon);
    void allowNotifyChanged(bool state);
    void notifySoundChanged(bool state);
    void lockShowNotifyChanged(bool state);
    void showInNotifyCenterChanged(bool state);
    void showNotifyPreviewChanged(bool state);
    void showOnDesktop(bool state);

private:
    NotificationSetting *m_setting;
    QString m_softName;//应用程序名
    QString m_icon;//应用系统图表
    QString m_actName;//传入后端应用名
    bool m_isAllowNotify;//允许应用通知
    bool m_isNotifySound;//是否有通知声音
    bool m_isLockShowNotify;//锁屏显示通知
    bool m_isShowDesktop;//show desktop notification
    bool m_isShowInNotifyCenter;//通知仅在通知中心显示
    bool m_isShowNotifyPreview;//显示消息预览
};
}
