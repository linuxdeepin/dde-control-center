//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "datetimemodel.h"
#include "langregionmodel.h"

namespace dccV25 {
LangRegionModel::LangRegionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int LangRegionModel::rowCount(const QModelIndex &) const
{
    DatetimeModel *sourceMode = dynamic_cast<DatetimeModel *>(parent());
    if (!sourceMode)
        return 0;

    return sourceMode->regions().size();
}

QVariant LangRegionModel::data(const QModelIndex &index, int role) const
{
    DatetimeModel *sourceMode = dynamic_cast<DatetimeModel *>(parent());
    if (!sourceMode)
        QVariant();

    static QStringList datas;
    const auto& regions = sourceMode->regions();
    if (datas.size() != regions.size()) {
        datas = sourceMode->languagesAndRegions();
    }

    QString key = regions.keys().value(index.row());
    QLocale locale = regions.value(key);
    RegionFormat regionFormat = RegionProxy::regionFormat(locale);

    const QDate CurrentDate(2024, 1, 1);
    const QTime CurrentTime(1, 1, 1);

    // maybe we should add to PinYinRole to sort Chinese words
    switch (role) {
    case Qt::DisplayRole:
        return datas.value(index.row());
    case SearchTextRole:
        return datas.value(index.row()) + key;
    case RegionKeyIdRole:
        return key;
    case LocaleKeyIdRole:
        return locale.name();
    case FirstDayOfWeek:
        return locale.standaloneDayName(regionFormat.firstDayOfWeekFormat);
    case ShortDate:
        return locale.toString(CurrentDate, regionFormat.shortDateFormat);
    case LongDate:
        return locale.toString(CurrentDate, regionFormat.longDateFormat);
    case ShortTime:
        return locale.toString(CurrentTime, regionFormat.shortTimeFormat);
    case LongTime:
        return locale.toString(CurrentTime, regionFormat.longTimeFormat);
    case Currency:
        return regionFormat.currencyFormat.toUtf8();
    case Digit:
        return regionFormat.numberFormat.toUtf8();
    case PaperSize:
        return regionFormat.paperFormat.toUtf8();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> LangRegionModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[SearchTextRole] = "searchTextRole";
    names[RegionKeyIdRole] = "langKey";
    names[LocaleKeyIdRole] = "localeKey";
    names[FirstDayOfWeek] = "firstDay";
    names[ShortDate] = "shortDate";
    names[LongDate] = "longDate";
    names[ShortTime] = "shortTime";
    names[LongTime] = "longTime";
    names[Currency] = "currency";
    names[Digit] = "digit";
    names[PaperSize] = "paperSize";
    return names;
}
}
