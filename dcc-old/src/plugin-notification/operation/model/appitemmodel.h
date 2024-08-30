//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QObject>
#include <QDBusVariant>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace DCC_NAMESPACE{

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
        SHOWINNOTIFICATIONCENTER,
        LOCKSCREENSHOWNOTIFICATION
    } AppConfigurationItem;

    explicit AppItemModel(QObject *parent = nullptr);

    inline QString getAppName()const {return  m_softName;}
    void setSoftName(const QString &name);

    inline QString getIcon()const {return m_icon;}
    void setIcon(const QString &icon);

    inline bool isAllowNotify()const {return m_isAllowNotify;}
    void setAllowNotify(const bool &state);

    inline bool isNotifySound()const {return m_isNotifySound;}
    void setNotifySound(const bool &state);

    inline bool isLockShowNotify()const {return m_isLockShowNotify;}
    void setLockShowNotify(const bool &state);

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

private:
    QString m_softName;//应用程序名
    QString m_icon;//应用系统图表
    QString m_actName;//传入后端应用名
    bool m_isAllowNotify;//允许应用通知
    bool m_isNotifySound;//是否有通知声音
    bool m_isLockShowNotify;//锁屏显示通知
    bool m_isShowInNotifyCenter;//通知仅在通知中心显示
    bool m_isShowNotifyPreview;//显示消息预览
};
}
