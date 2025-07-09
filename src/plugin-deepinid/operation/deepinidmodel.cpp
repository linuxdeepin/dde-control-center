// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deepinidmodel.h"

#include <QDateTime>
#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DeepinIDModel, "dcc-deepinid-model");

DeepinidModel::DeepinidModel(QObject *parent)
    : QObject{ parent }
    , m_loginState(false)
    , m_activation(false)
    , m_syncEnabled(true)
    , m_syncSwitch(false)
    , m_syncItemShow(false)
    , m_syncInfoListModel(new SyncInfoListModel(this))
    , m_appInfoListModel(new AppInfoListModel(this))
    , m_downloader(nullptr)
{

}

void DeepinidModel::setUserinfo(const QVariantMap &userinfo)
{
    if (m_userinfo == userinfo)
        return;

    m_userinfo = userinfo;
    m_loginState = !m_userinfo["Username"].toString().isEmpty();
    m_region = (m_userinfo["Region"].toString() == "CN") ? tr("Mainland China") : tr("Other regions");
    m_userName = m_userinfo["Nickname"].toString();
    if (m_userName.isEmpty()) {
        m_userName = m_userinfo["Username"].toString();
    }
    m_wechatName = m_userinfo["WechatNickname"].toString().trimmed();

    updateAvatarPath();

    Q_EMIT loginStateChanged(m_loginState);
    Q_EMIT avatarChanged(m_avatar);
    Q_EMIT regionChanged(m_region);
    Q_EMIT userNameChanged(m_userName);
    Q_EMIT wechatNameChanged(m_wechatName);
    Q_EMIT syncEnabledChanged(syncEnabled());
}

void DeepinidModel::setActivation(bool activation)
{
    if (m_activation == activation)
        return;

    m_activation = activation;
    Q_EMIT syncEnabledChanged(syncEnabled());
}

bool DeepinidModel::syncEnabled() const
{
    return m_activation && (m_userinfo["Region"].toString() == "CN");
}

void DeepinidModel::setSyncSwitch(bool enable)
{
    if (m_syncSwitch == enable)
        return;

    m_syncSwitch = enable;
    Q_EMIT syncSwitchChanged(m_syncSwitch);
}

void DeepinidModel::setSyncItemShow(bool show)
{
    if (m_syncItemShow == show)
        return;

    m_syncItemShow = show;
    Q_EMIT syncItemShowChanged(m_syncItemShow);
}

void DeepinidModel::setLastSyncTime(const qlonglong &lastSyncTime)
{
    if (m_syncTimestamp == lastSyncTime)
        return;

    m_lastSyncTime = QDateTime::fromMSecsSinceEpoch(lastSyncTime * 1000).toString("yyyy/MM/dd hh:mm");
    Q_EMIT lastSyncTimeChanged(m_lastSyncTime);
}

void DeepinidModel::updateSyncItem(const QString &key, bool enable)
{
    m_syncInfoListModel->updateSyncItem(key, enable);
}

void DeepinidModel::initAppItemList(QList<AppItemData*> appItemList)
{
    m_appInfoListModel->clearItem();
    for (auto item : appItemList) {
        m_appInfoListModel->addAppItem(item);
    }
}

void DeepinidModel::updateAppItem(const QString &key, bool enable)
{
    m_appInfoListModel->updateAppItem(key, enable);
}

QString DeepinidModel::warnTipsMessage()
{
    if (!m_activation) {
        return tr("The feature is not available at present, please activate your system first");
    };

    if (m_userinfo["Region"].toString() != "CN") {
        return tr("Subject to your local laws and regulations, it is currently unavailable in your region.");
    }

    return QString();
}

void DeepinidModel::updateAvatarPath()
{
    QString profile_image = m_userinfo["ProfileImage"].toString();
    if (profile_image.isEmpty()) {
        return;
    }

    QString avatarPath(QString("%1/.cache/deepin/dde-control-center/sync%2").arg(getenv("HOME")).arg(getUserName()));
    QDir dir;
    dir.mkpath(avatarPath);
    qCDebug(DeepinIDModel) << "profile image:" << profile_image << ", avatar path:" << avatarPath;

    QString localAvatar = avatarPath + profile_image.right(profile_image.size() - profile_image.lastIndexOf("/"));
    QString localDefault = avatarPath + "/default.svg";
    if (QFile::exists(localAvatar)) {
        qCDebug(DeepinIDModel) << "local Avatar:" << localAvatar;
        m_avatar = localAvatar;
        return;
    } else if (QFile::exists(localDefault)) {
        qCDebug(DeepinIDModel) << "local default:" << localDefault;
        m_avatar = localDefault;
        return;
    }

    if (m_downloader == nullptr) {
        m_downloader = new DownloadUrl(this);
        connect(m_downloader, &DownloadUrl::fileDownloaded, this, &DeepinidModel::updateAvatarPath, Qt::UniqueConnection);
    }
    m_downloader->downloadFileFromURL(profile_image, avatarPath);
}
