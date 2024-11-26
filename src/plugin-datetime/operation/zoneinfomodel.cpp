// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "zoneinfomodel.h"
#include "datetimemodel.h"

namespace dccV25 {

UserTimezoneModel::UserTimezoneModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

UserTimezoneModel::~UserTimezoneModel()
{

}

int UserTimezoneModel::rowCount(const QModelIndex &) const
{
    DatetimeModel *sourceMode = dynamic_cast<DatetimeModel *>(parent());
    if (!sourceMode)
        return 0;

    auto zones = sourceMode->userTimeZones();
    return zones.count();
}

QVariant UserTimezoneModel::data(const QModelIndex &index, int role) const
{
    DatetimeModel *sourceMode = dynamic_cast<DatetimeModel *>(parent());
    if (!sourceMode)
        return QVariant();

    auto zones = sourceMode->userTimeZones();
    if (!index.isValid() || index.row() >= zones.size())
        return QVariant();

    const auto &zoneInfo = zones.value(index.row());
    const QString &zoneId = zoneInfo.getZoneName();
    const QString &zoneDisplay = sourceMode->zoneDisplayName(zoneId);
    const QString &description = sourceMode->timeZoneDescription(zoneInfo);

    switch (role) {
    case Qt::DisplayRole:
        return zoneDisplay; // (UTC+08:00)上海
    case DescriptionRole:
        return description; // xxx hours earlier/later than local
    case ZoneIdRole:
        return zoneId;  //  Asia/Shanghai
    case ShiftRole:
        return zoneInfo.getUTCOffset() / 3600;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> UserTimezoneModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[DescriptionRole] = "description";
    names[ShiftRole] = "shift";
    names[ZoneIdRole] = "zoneId";
    return names;
}

void UserTimezoneModel::reset()
{
    beginResetModel();
    endResetModel();
}

///////////////////////////////////////////////
ZoneInfoModel::ZoneInfoModel(QObject *parent)
    :QAbstractListModel(parent)
{

}

ZoneInfoModel::~ZoneInfoModel()
{

}

int ZoneInfoModel::rowCount(const QModelIndex &) const
{
    DatetimeModel *sourceMode = dynamic_cast<DatetimeModel *>(parent());
    if (!sourceMode)
        return 0;

    auto list = sourceMode->zoneIdList();

    return list.size();
}

QVariant ZoneInfoModel::data(const QModelIndex &index, int role) const
{
    DatetimeModel *sourceMode = dynamic_cast<DatetimeModel *>(parent());
    if (!sourceMode)
        return QVariant();

    auto list = sourceMode->zoneIdList();

    if (!index.isValid() || index.row() >= list.size())
        return QVariant();

    const QString &zoneId = list.value(index.row());   // "Asia/Shanghai"
    const QString &zoneDisplay = sourceMode->zoneDisplayName(zoneId); // (UTC+08:00)上海

    switch (role) {
    case Qt::DisplayRole:
        return zoneDisplay; // (UTC+08:00)上海
    case SearchTextRole:
        return zoneDisplay + zoneId; // + (UTC+08:00)上海 Asia/Shanghai
    case ZoneIdRole:
        return zoneId;  //  Asia/Shanghai
    case CityNameRole:
        return zoneDisplay.mid(zoneDisplay.indexOf(" ") + 1); // 上海
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> ZoneInfoModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[SearchTextRole] = "searchText";
    names[ZoneIdRole] = "zoneId";
    names[CityNameRole] = "cityName";
    return names;
}

} // namespace dccV25
