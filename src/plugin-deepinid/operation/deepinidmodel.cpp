// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deepinidmodel.h"

DeepinidModel::DeepinidModel(QObject *parent)
    : QObject{ parent }
    , m_loginState(false)
    , m_activation(false)
    , m_enableSync(false)
{
}

void DeepinidModel::setUserinfo(const QVariantMap &userinfo)
{
    if (m_userinfo == userinfo)
        return;

    m_userinfo = userinfo;
    qDebug() << "userinfo:" << userinfo;

    m_loginState = !m_userinfo["Username"].toString().isEmpty();
    m_region = (m_userinfo["Region"].toString() == "CN") ? tr("Mainland China") : tr("Other regions");
    m_displayName = m_userinfo["Nickname"].toString();
    if (m_displayName.isEmpty()) {
        m_displayName = m_userinfo["Username"].toString();
    }

    // TODO: 直接使用网络图片，暂时没做本地缓存，后面再考虑
    m_avatar = m_userinfo["ProfileImage"].toString();

    emit loginStateChanged(m_loginState);
    emit regionChanged(m_region);
    emit displayNameChanged(m_displayName);
}

void DeepinidModel::setActivation(bool activation)
{
    if (m_activation == activation)
        return;

    m_activation = activation;
    Q_EMIT activationChanged(m_activation);
}

void DeepinidModel::setEnableSync(bool enableSync)
{
    if (m_enableSync == enableSync) 
        return;

    m_enableSync = enableSync;
    Q_EMIT enableSyncChanged(m_enableSync);
}
