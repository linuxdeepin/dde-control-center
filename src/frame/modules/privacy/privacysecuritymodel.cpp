// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "applicationitem.h"
#include "privacysecuritydataproxy.h"
#include "privacysecuritymodel.h"

#include <QStandardPaths>
#include <QDebug>
PrivacySecurityModel::PrivacySecurityModel(QObject *parent)
    : QObject(parent)
    , m_uniqueID(1)
    , m_updating(true)
{
}

bool PrivacySecurityModel::isPremissionEnabled(int premission) const
{
    return m_premissionMap.value(premission, PrivacySecurityDataProxy::AllEnable) != PrivacySecurityDataProxy::AllDisabled;
}

void PrivacySecurityModel::setPremissionEnabled(int premission, bool enabled)
{
    // worker 实现
    if (enabled) {
        if (!m_appItems.isEmpty()) {
            auto &&item = m_appItems.first();
            qInfo() << item << premission;
            // 触发设置信号
            switch (premission) {
            case PermissionType::CameraPermission:
                item->setPremissionEnabled(PermissionType::CameraPermission, item->isPremissionEnabled(PermissionType::CameraPermission));
                break;
            case PermissionType::FoldersPermission:
                item->setPremissionEnabled(PermissionType::DocumentsFoldersPermission, item->isPremissionEnabled(PermissionType::DocumentsFoldersPermission));
                item->setPremissionEnabled(PermissionType::PicturesFoldersPermission, item->isPremissionEnabled(PermissionType::PicturesFoldersPermission));
                break;
            }
        }
    } else {
        Q_EMIT requestSetPremissionMode(premission, PrivacySecurityDataProxy::AllDisabled);
    }
}

bool PrivacySecurityModel::addApplictionItem(ApplicationItem *item)
{
    auto it = std::find_if(m_appItems.begin(), m_appItems.end(), [item](ApplicationItem *appItem) {
        return appItem->id() == item->id();
    });
    if (it == m_appItems.end()) {
        // 排序添加
        auto before = std::find_if(m_appItems.begin(), m_appItems.end(), [item](ApplicationItem *appItem) {
            return appItem->sortField() > item->sortField();
        });
        item->setUniqueID(createUniqueID());
        connect(item, &ApplicationItem::dataChanged, this, &PrivacySecurityModel::onItemDataChanged);
        Q_EMIT itemAboutToBeAdded(before - m_appItems.begin());
        m_appItems.insert(before, item);
        Q_EMIT itemAdded();
        return true;
    }
    return false;
}

void PrivacySecurityModel::removeApplictionItem(const QString &id)
{
    auto it = std::find_if(m_appItems.begin(), m_appItems.end(), [id](ApplicationItem *appItem) {
        return appItem->id() == id;
    });
    if (it != m_appItems.end()) {
        Q_EMIT itemAboutToBeRemoved(it - m_appItems.begin());
        m_appItems.erase(it);
        Q_EMIT itemRemoved();
    }
}

void PrivacySecurityModel::dataUpdateFinished(bool updating)
{
    m_updating = updating;
    if (m_updating) {
        Q_EMIT itemDataUpdate(m_updating);
        blockSignals(m_updating);
    } else {
        blockSignals(m_updating);
        Q_EMIT itemDataUpdate(m_updating);
    }
}

ApplicationItem *PrivacySecurityModel::applictionItem(unsigned uniqueID)
{
    auto it = std::find_if(m_appItems.begin(), m_appItems.end(), [uniqueID](ApplicationItem *item) {
        return item->getUniqueID() == uniqueID;
    });
    return it == m_appItems.end() ? nullptr : *it;
}

int PrivacySecurityModel::getAppIndex(unsigned uniqueID) const
{
    auto it = std::find_if(m_appItems.begin(), m_appItems.end(), [uniqueID](ApplicationItem *item) {
        return item->getUniqueID() == uniqueID;
    });
    return it == m_appItems.end() ? -1 : it - m_appItems.begin();
}

void PrivacySecurityModel::updatePermission()
{
    for (auto &&item : m_appItems) {
        updatePermission(item);
    }
}

void PrivacySecurityModel::updatePermission(ApplicationItem *item)
{
    item->onPremissionEnabledChanged(PermissionType::DocumentsFoldersPermission, !m_blacklist[premissiontoPath(PermissionType::DocumentsFoldersPermission)].contains(item->appPath()));
    item->onPremissionEnabledChanged(PermissionType::PicturesFoldersPermission, !m_blacklist[premissiontoPath(PermissionType::PicturesFoldersPermission)].contains(item->appPath()));
    item->onPremissionEnabledChanged(PermissionType::CameraPermission, !m_blacklist[premissiontoPath(PermissionType::CameraPermission)].contains(item->appPath()));
}

const QStringList PrivacySecurityModel::blacklist(const QString &file) const
{
    return m_blacklist.value(file);
}

const QString PrivacySecurityModel::premissiontoPath(int premission) const
{
    switch (premission) {
    case PermissionType::CameraPermission:
        return "camera";
    case PermissionType::DocumentsFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    case PermissionType::PicturesFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
    return QString();
}

int PrivacySecurityModel::pathtoPremission(const QString &path, bool mainPremission) const
{
    if (path == "camera") {
        return PermissionType::CameraPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) {
        return mainPremission ? PermissionType::FoldersPermission : PermissionType::DocumentsFoldersPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)) {
        return mainPremission ? PermissionType::FoldersPermission : PermissionType::PicturesFoldersPermission;
    }
    return 0;
}

void PrivacySecurityModel::onPremissionModeChanged(int premission, int mode)
{
    if (m_premissionMap.contains(premission) && m_premissionMap.value(premission) == mode)
        return;
    m_premissionMap[premission] = mode;
    Q_EMIT premissionEnabledChanged(premission, isPremissionEnabled(premission));
}

void PrivacySecurityModel::onAppPremissionEnabledChanged(const QString &file, const QStringList &apps)
{
    if (isPremissionEnabled(pathtoPremission(file, true))) {
        m_blacklist[file] = apps;
        if (m_cacheBlacklist[file] != apps) {
            m_cacheBlacklist[file] = apps;
            Q_EMIT requestUpdateCacheBlacklist(m_cacheBlacklist);
        }
    } else {
        m_blacklist[file] = m_cacheBlacklist[file];
    }
    updatePermission();
}

void PrivacySecurityModel::onItemDataChanged()
{
    ApplicationItem *item = qobject_cast<ApplicationItem *>(sender());
    Q_EMIT itemDataChanged(item);
}

void PrivacySecurityModel::onCacheBlacklistChanged(const QMap<QString, QStringList> &cacheBlacklist)
{
    m_cacheBlacklist = cacheBlacklist;
}

unsigned PrivacySecurityModel::createUniqueID()
{
    return m_uniqueID++;
}
