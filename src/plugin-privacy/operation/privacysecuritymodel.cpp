// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "privacysecuritymodel.h"
#include "applicationitem.h"
#include "privacysecuritydataproxy.h"
#include <QStandardPaths>

AppsModel::AppsModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariant AppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
        case NameRole: {
            return m_appItems.at(index.row())->name();
        }
        case IconNameRole: {
            return m_appItems.at(index.row())->icon();
        }
        case CameraRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::CameraPermission);
        }
        case DocumentRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::DocumentFoldersPermission);
        }
        case PictureRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::PictureFoldersPermission);
        }
        case DesktopRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::DesktopFoldersPermission);
        }
        case VideoRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::VideoFoldersPermission);
        }
        case MusicRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::MusicFoldersPermission);
        }
        case DownloadRole: {
            return m_appItems.at(index.row())->isPremissionEnabled(ApplicationItem::DownloadFoldersPermission);
        }
    }
    return QVariant();
}

QVariant AppsModel::data(const QModelIndex &index, const QString propertyName) const
{
    const auto roleNames = this->roleNames();
    for (auto it = roleNames.cbegin(); it != roleNames.cend(); ++it) {
        if (propertyName == it.value()) {
            return data(index, it.key());
        }
    }
    return QVariant();
}

int AppsModel::rowCount(const QModelIndex &) const
{
    return m_appItems.size();
}

QHash<int, QByteArray> AppsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[NameRole] = "name";
    roleNames[IconNameRole] = "iconName";
    roleNames[CameraRole] = "cameraPermission";
    roleNames[DocumentRole] = "documentPermission";
    roleNames[PictureRole] = "picturePermission";
    roleNames[DesktopRole] = "desktopPermission";
    roleNames[VideoRole] = "videoPermission";
    roleNames[MusicRole] = "musicPermission";
    roleNames[DownloadRole] = "downloadPermission";
    return roleNames;
}

void AppsModel::reset(ApplicationItemListPtr appList)
{
    beginResetModel();
    m_appItems =  appList;
    endResetModel();
}

void AppsModel::appendItem(ApplicationItem *appItem)
{
    int insertPos = m_appItems.size();
    for (int i = 0; i < m_appItems.size(); ++i) {
        if (appItem->sortField() < m_appItems.at(i)->sortField()) {
            insertPos = i;
            break;
        }
    }

    beginInsertRows(QModelIndex(), insertPos, insertPos);
    m_appItems.insert(insertPos, appItem);
    endInsertRows();
}

void AppsModel::removeItem(ApplicationItem *appItem)
{
    for (int index = 0; index < m_appItems.size(); index++) {
        if (m_appItems.at(index) == appItem) {
            beginRemoveRows(QModelIndex(), index, index);
            m_appItems.removeAt(index);
            endRemoveRows();
            return;
        }
    }
}

PrivacySecurityModel::PrivacySecurityModel(QObject *parent)
    : QObject(parent)
    , m_appModel(new AppsModel(this))
    , m_uniqueID(1)
    , m_updating(true)
{

}

PrivacySecurityModel::~PrivacySecurityModel()
{

}

bool PrivacySecurityModel::isPremissionEnabled(int premission) const
{
    switch (premission) {
    case ApplicationItem::FoldersPermission:
        return isPremissionEnabled(ApplicationItem::DocumentFoldersPermission) 
            && isPremissionEnabled(ApplicationItem::PictureFoldersPermission)
            && isPremissionEnabled(ApplicationItem::DesktopFoldersPermission)
            && isPremissionEnabled(ApplicationItem::VideoFoldersPermission)
            && isPremissionEnabled(ApplicationItem::MusicFoldersPermission)
            && isPremissionEnabled(ApplicationItem::DownloadFoldersPermission);
    default:
        return m_premissionMap.value(premission, PrivacySecurityDataProxy::AllEnable) != PrivacySecurityDataProxy::AllDisabled;
    }
}

void PrivacySecurityModel::setPremissionEnabled(int premission, bool enabled)
{
    // worker 实现
    if (enabled) {
        if (!m_appModel->appList().isEmpty()) {
            auto &&item = m_appModel->appList().first();
            qCInfo(DCC_PRIVACY) << "Set premission enabled: true, item: " << item << ", premission: " << premission;
            // 触发设置信号
            switch (premission) {
            case ApplicationItem::CameraPermission:
                item->setPremissionEnabled(ApplicationItem::CameraPermission, item->isPremissionEnabled(ApplicationItem::CameraPermission));
                break;
            case ApplicationItem::FoldersPermission:
                item->setPremissionEnabled(ApplicationItem::DocumentFoldersPermission, item->isPremissionEnabled(ApplicationItem::DocumentFoldersPermission));
                item->setPremissionEnabled(ApplicationItem::PictureFoldersPermission, item->isPremissionEnabled(ApplicationItem::PictureFoldersPermission));
                item->setPremissionEnabled(ApplicationItem::DesktopFoldersPermission, item->isPremissionEnabled(ApplicationItem::DesktopFoldersPermission));
                item->setPremissionEnabled(ApplicationItem::VideoFoldersPermission, item->isPremissionEnabled(ApplicationItem::VideoFoldersPermission));
                item->setPremissionEnabled(ApplicationItem::MusicFoldersPermission, item->isPremissionEnabled(ApplicationItem::MusicFoldersPermission));
                item->setPremissionEnabled(ApplicationItem::DownloadFoldersPermission, item->isPremissionEnabled(ApplicationItem::DownloadFoldersPermission));
                break;
            }
        }
    } else {
        Q_EMIT requestSetPremissionMode(premission, PrivacySecurityDataProxy::AllDisabled);
    }
}

bool PrivacySecurityModel::addApplictionItem(ApplicationItem *item)
{
    m_appModel->appendItem(item);
    return true;
}

void PrivacySecurityModel::removeApplictionItem(const QString &id)
{
    const auto list = m_appModel->appList();
    auto it = std::find_if(list.cbegin(), list.cend(), [id](ApplicationItem *appItem) {
        return appItem->id() == id;
    }); 
    if (it != list.end()) {
        Q_EMIT itemAboutToBeRemoved(it - list.begin());
        m_appModel->removeItem(*it);
        delete *it;
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
    auto it = std::find_if(m_appModel->appList().begin(), m_appModel->appList().end(), [uniqueID](ApplicationItem *item) {
        return item->getUniqueID() == uniqueID;
    });
    return it == m_appModel->appList().end() ? nullptr : *it;
}

int PrivacySecurityModel::getAppIndex(unsigned uniqueID) const
{
    auto it = std::find_if(m_appModel->appList().begin(), m_appModel->appList().end(), [uniqueID](ApplicationItem *item) {
        return item->getUniqueID() == uniqueID;
    });
    return it == m_appModel->appList().end() ? -1 : it - m_appModel->appList().begin();
}

void PrivacySecurityModel::updatePermission()
{
    for (int i = 0; i < m_appModel->appList().count(); ++i) {
        if (updatePermission(m_appModel->appList()[i])) {
            Q_EMIT appModel()->dataChanged(m_appModel->index(i), m_appModel->index(i));
        }
    }
}

bool PrivacySecurityModel::updatePermission(ApplicationItem *item)
{
    bool sucUpdate = false;
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::DocumentFoldersPermission, !m_blacklist[premissiontoPath(ApplicationItem::DocumentFoldersPermission)].contains(item->appPath()));
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::PictureFoldersPermission, !m_blacklist[premissiontoPath(ApplicationItem::PictureFoldersPermission)].contains(item->appPath()));
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::DesktopFoldersPermission, !m_blacklist[premissiontoPath(ApplicationItem::DesktopFoldersPermission)].contains(item->appPath()));
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::VideoFoldersPermission, !m_blacklist[premissiontoPath(ApplicationItem::VideoFoldersPermission)].contains(item->appPath()));
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::MusicFoldersPermission, !m_blacklist[premissiontoPath(ApplicationItem::MusicFoldersPermission)].contains(item->appPath()));
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::DownloadFoldersPermission, !m_blacklist[premissiontoPath(ApplicationItem::DownloadFoldersPermission)].contains(item->appPath()));
    sucUpdate |= item->onPremissionEnabledChanged(ApplicationItem::CameraPermission, !m_blacklist[premissiontoPath(ApplicationItem::CameraPermission)].contains(item->appPath()));
    return sucUpdate;
}

const QSet<QString> PrivacySecurityModel::blacklist(const QString &file) const
{
    return m_blacklist.value(file);
}

const QString PrivacySecurityModel::premissiontoPath(int premission) const
{
    switch (premission) {
    case ApplicationItem::CameraPermission:
        return "camera";
    case ApplicationItem::DocumentFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    case ApplicationItem::PictureFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    case ApplicationItem::DesktopFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    case ApplicationItem::VideoFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    case ApplicationItem::MusicFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    case ApplicationItem::DownloadFoldersPermission:
        return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    }
    
    return QString();
}

int PrivacySecurityModel::pathtoPremission(const QString &path, bool mainPremission) const
{
    if (path == "camera") {
        return ApplicationItem::CameraPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)) {
        return mainPremission ? ApplicationItem::FoldersPermission : ApplicationItem::DocumentFoldersPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)) {
        return mainPremission ? ApplicationItem::FoldersPermission : ApplicationItem::PictureFoldersPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)) {
        return mainPremission ? ApplicationItem::FoldersPermission : ApplicationItem::DesktopFoldersPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)) {
        return mainPremission ? ApplicationItem::FoldersPermission : ApplicationItem::VideoFoldersPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::MusicLocation)) {
        return mainPremission ? ApplicationItem::FoldersPermission : ApplicationItem::MusicFoldersPermission;
    } else if (path == QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)) {
        return mainPremission ? ApplicationItem::FoldersPermission : ApplicationItem::DownloadFoldersPermission;
    }
    return 0;
}

void PrivacySecurityModel::emitAppDataChanged(ApplicationItem *item)
{
    for (int i = 0; i < m_appModel->appList().count(); ++i) {
        if (m_appModel->appList()[i] == item) {
            Q_EMIT appModel()->dataChanged(m_appModel->index(i), m_appModel->index(i));
            return;
        }
    }
}

void PrivacySecurityModel::onPremissionModeChanged(int premission, int mode)
{
    int groupPremisson = premission & ApplicationItem::GroupPermissionMask;
    m_premissionMap[groupPremisson] = mode;
    emitPremissionModeChanged(groupPremisson);
}

void PrivacySecurityModel::emitPremissionModeChanged(int premission)
{
    int groupPremisson = premission & ApplicationItem::GroupPermissionMask;
    Q_EMIT premissionEnabledChanged(groupPremisson, isPremissionEnabled(groupPremisson));
}

void PrivacySecurityModel::onAppPremissionEnabledChanged(const QString &file, const QSet<QString> &apps)
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

void PrivacySecurityModel::onItemPermissionChanged()
{
    ApplicationItem *item = qobject_cast<ApplicationItem *>(sender());
    if (item)
        updatePermission(item);
}

void PrivacySecurityModel::onItemDataChanged()
{
    ApplicationItem *item = qobject_cast<ApplicationItem *>(sender());
    Q_EMIT itemDataChanged(item);
}

void PrivacySecurityModel::onCacheBlacklistChanged(const QMap<QString, QSet<QString>> &cacheBlacklist)
{
    m_cacheBlacklist = cacheBlacklist;
}

unsigned PrivacySecurityModel::createUniqueID()
{
    return m_uniqueID++;
}
