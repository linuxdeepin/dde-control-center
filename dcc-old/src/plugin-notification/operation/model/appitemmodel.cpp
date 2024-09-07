//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "appitemmodel.h"

#include <QVariant>

using namespace DCC_NAMESPACE;

AppItemModel::AppItemModel(QObject *parent)
    : QObject(parent)
    , m_softName(QString())
    , m_isAllowNotify(false)
    , m_isNotifySound(false)
    , m_isLockShowNotify(false)
    , m_isShowInNotifyCenter(false)
    , m_isShowNotifyPreview(false)
{

}

void AppItemModel::setActName(const QString &name)
{
    if (m_actName != name) {
        m_actName = name;
    }
}

void AppItemModel::onSettingChanged(const QString &id, const uint &item, QDBusVariant var)
{
    if (id != m_actName)
        return;
    switch (item) {
    case APPNAME:
        setSoftName(var.variant().toString());
        break;
    case APPICON:
        setIcon(var.variant().toString());
        break;
    case ENABELNOTIFICATION:
        setAllowNotify(var.variant().toBool());
        break;
    case ENABELPREVIEW:
        setShowNotifyPreview(var.variant().toBool());
        break;
    case ENABELSOUND:
        setNotifySound(var.variant().toBool());
        break;
    case SHOWINNOTIFICATIONCENTER:
        setShowInNotifyCenter(var.variant().toBool());
        break;
    case LOCKSCREENSHOWNOTIFICATION:
        setLockShowNotify(var.variant().toBool());
        break;
    }
}

void AppItemModel::setSoftName(const QString &name) {
    if (m_softName == name)
        return;
    m_softName = name;
    Q_EMIT softNameChanged(name);
}

void AppItemModel::setIcon(const QString &icon)
{
    if (m_icon == icon)
        return;
    m_icon = icon;
    Q_EMIT iconChanged(icon);
}

void AppItemModel::setAllowNotify(const bool &state)
{
    if (m_isAllowNotify == state)
        return;
    m_isAllowNotify = state;
    Q_EMIT allowNotifyChanged(state);

}

void AppItemModel::setNotifySound(const bool &state)
{
    if (m_isNotifySound == state)
        return;
    m_isNotifySound = state;
    Q_EMIT notifySoundChanged(state);
}

void AppItemModel::setLockShowNotify(const bool &state)
{
    if (m_isLockShowNotify == state)
        return;
    m_isLockShowNotify = state;
    Q_EMIT lockShowNotifyChanged(state);
}

void AppItemModel::setShowInNotifyCenter(const bool &state)
{
    if (m_isShowInNotifyCenter == state)
        return;
    m_isShowInNotifyCenter = state;
    Q_EMIT showInNotifyCenterChanged(state);
}

void AppItemModel::setShowNotifyPreview(const bool &state)
{
    if (m_isShowNotifyPreview == state)
        return;
    m_isShowNotifyPreview = state;
    Q_EMIT showNotifyPreviewChanged(state);
}
