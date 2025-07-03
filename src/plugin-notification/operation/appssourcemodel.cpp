// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "appssourcemodel.h"
#include "model/appitemmodel.h"
#include <qlogging.h>

#include <DPinyin>

using namespace DCC_NAMESPACE;

AppsSourceModel::AppsSourceModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QHash<int, QByteArray> AppsSourceModel::roleNames() const
{
    QHash<int, QByteArray> names= QAbstractItemModel::roleNames();
    names[AppIdRole] = "AppId";
    names[AppNameRole] = "AppName";
    names[AppIconRole] = "AppIcon";
    names[EnableNotificationRole] = "EnableNotification";
    names[EnablePreviewRole] = "EnablePreview";
    names[EnableSoundRole] = "EnableSound";
    names[ShowNotificationDesktopRole] = "ShowNotificationDesktop";
    names[ShowNotificationCenterRole] = "ShowNotificationCenter";
    names[LockScreenShowNotificationRole] = "LockScreenShowNotification";
    names[TransliteratedRole] = "Transliterated";
    return names;
}

QModelIndex AppsSourceModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= m_appItemModels.size()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex AppsSourceModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int AppsSourceModel::rowCount(const QModelIndex &) const
{
    return m_appItemModels.size();
}

int AppsSourceModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant AppsSourceModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_appItemModels.size()) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole:
        return QVariant::fromValue(m_appItemModels.at(index.row()));
    case AppIdRole:
        return m_appItemModels.at(index.row())->getActName();
    case AppNameRole:
        return m_appItemModels.at(index.row())->softName();
    case AppIconRole:
        return m_appItemModels.at(index.row())->icon();
    case EnableNotificationRole:
        return m_appItemModels.at(index.row())->isAllowNotify();
    case EnablePreviewRole:
        return m_appItemModels.at(index.row())->isShowNotifyPreview();
    case EnableSoundRole:
        return m_appItemModels.at(index.row())->isNotifySound();
    case ShowNotificationDesktopRole:
        return m_appItemModels.at(index.row())->isShowDesktop();
    case ShowNotificationCenterRole:
        return m_appItemModels.at(index.row())->isShowInNotifyCenter();
    case LockScreenShowNotificationRole:
        return m_appItemModels.at(index.row())->isLockShowNotify();

    case TransliteratedRole: {
        const auto decodedDisplay = Dtk::Core::pinyin(index.data(AppNameRole).toString(), Dtk::Core::TS_NoneTone);
        if (decodedDisplay.isEmpty()) return QString();
        const QString transliterated = decodedDisplay.constFirst();
        if (transliterated.isEmpty()) return transliterated;
        const QChar & firstChar = transliterated.constData()[0];
        if (firstChar.isDigit()) return QString("#%1").arg(transliterated);
        else if (!firstChar.isLetter()) return QString("&%1").arg(transliterated);
        return transliterated;
    }
    default:
        break;
    }
    return QVariant();
}

bool AppsSourceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int i = index.row();
    if (i < 0 || i >= m_appItemModels.size()) {
        return false;
    }

    switch (role) {
    case EnableNotificationRole: {
        m_appItemModels.at(index.row())->setAllowNotify(value.toBool());
        return true;
    }
    case EnablePreviewRole: {
        m_appItemModels.at(index.row())->setShowNotifyPreview(value.toBool());
        return true;
    }
    case EnableSoundRole: {
        m_appItemModels.at(index.row())->setNotifySound(value.toBool());
        return true;
    }
    case ShowNotificationDesktopRole: {
        m_appItemModels.at(index.row())->setShowDesktop(value.toBool());
        return true;
    }
    case ShowNotificationCenterRole: {
        m_appItemModels.at(index.row())->setShowInNotifyCenter(value.toBool());
        return true;
    }
    case LockScreenShowNotificationRole: {
        m_appItemModels.at(index.row())->setLockShowNotify(value.toBool());
        return true;
    }
    default:
        break;
    }
    return false;
}

void AppsSourceModel::appAdded(AppItemModel *item)
{
    beginInsertRows(QModelIndex(), m_appItemModels.size(), m_appItemModels.size());
    m_appItemModels.append(item);
    connect(item, &AppItemModel::allowNotifyChanged, this, [this, item]() {
        Q_EMIT dataChanged(createIndex(m_appItemModels.indexOf(item), 0), createIndex(m_appItemModels.indexOf(item), 0));
    });
    connect(item, &AppItemModel::notifySoundChanged, this, [this, item]() {
        Q_EMIT dataChanged(createIndex(m_appItemModels.indexOf(item), 0), createIndex(m_appItemModels.indexOf(item), 0));
    });
    connect(item, &AppItemModel::lockShowNotifyChanged, this,[this, item]() {
        Q_EMIT dataChanged(createIndex(m_appItemModels.indexOf(item), 0), createIndex(m_appItemModels.indexOf(item), 0));
    });
    connect(item, &AppItemModel::showInNotifyCenterChanged, this, [this, item]() {
        Q_EMIT dataChanged(createIndex(m_appItemModels.indexOf(item), 0), createIndex(m_appItemModels.indexOf(item), 0));
    });
    connect(item, &AppItemModel::showNotifyPreviewChanged, this, [this, item]() {
        Q_EMIT dataChanged(createIndex(m_appItemModels.indexOf(item), 0), createIndex(m_appItemModels.indexOf(item), 0));
    });
    connect(item, &AppItemModel::showOnDesktop, this, [this, item]() {
        Q_EMIT dataChanged(createIndex(m_appItemModels.indexOf(item), 0), createIndex(m_appItemModels.indexOf(item), 0));
    });
    endInsertRows();
}

void AppsSourceModel::appRemoved(const QString &appName)
{
    for (int i = 0; i < m_appItemModels.size(); i++) {
        if (m_appItemModels[i]->getActName() == appName) {
            // Q_EMIT appListRemoved(m_appItemModels[i]);
            beginRemoveRows(QModelIndex(), i, i);
            m_appItemModels[i]->deleteLater();
            m_appItemModels[i] = nullptr;
            m_appItemModels.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}
