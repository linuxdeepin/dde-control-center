// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef V20_DISPLAY_UTILS_H
#define V20_DISPLAY_UTILS_H

#include "interface/namespace.h"

#include <QMargins>
#include <QSize>
#include <QVariant>
#include <QSettings>
#include <QMetaMethod>
#include <DSysInfo>
#include <DPlatformHandle>

DCORE_USE_NAMESPACE

const QMargins ThirdPageContentsMargins(10, 10, 10, 10);
const QMargins ScrollAreaMargins(10, 10, 10, 10);
const QMargins ScrollAreaOffsetMargins(0, 0, 10, 0);
const QMargins ListViweItemMargin(10, 8, 10, 8);
const QVariant VListViewItemMargin = QVariant::fromValue(ListViweItemMargin);
const QMargins ThirdPageCmbMargins(0, 0, 0, 0);

const int ComboxWidgetHeight = 48;
const int SwitchWidgetHeight = 36;
const int ComboxTitleWidth = 110;

const QSize ListViweItemIconSize(84,84);
const QSize ListViweItemSize(170,168);
const QSize ListViweItemIconSize_ListMode(42,42);
const QSize ListViweItemSize_ListMode(168,48);
const QSize ListViweIconSize(32,32);
const QSize ListViweSysItemSize(168,58);

const qint32 ActionIconSize=30;//大图标角标大小
const qint32 ActionListSize=26;//list图标角标大小

const int List_Interval = 10;
const QMargins ArrowEnterClickMargin(8, 8, 8, 8);

const QString SystemTypeName = DSysInfo::uosEditionName(QLocale::c());

const DSysInfo::UosType UosType = DSysInfo::uosType();
const DSysInfo::UosEdition UosEdition = DSysInfo::uosEditionType();
const bool IsServerSystem = (DSysInfo::UosServer == UosType);//是否是服务器版
const bool IsCommunitySystem = (DSysInfo::UosCommunity == UosEdition);//是否是社区版
const bool IsProfessionalSystem = (DSysInfo::UosProfessional == UosEdition);//是否是专业版
const bool IsHomeSystem = (DSysInfo::UosHome == UosEdition);//是否是个人版
const bool IsDeepinDesktop = (DSysInfo::DeepinDesktop == DSysInfo::deepinType());//是否是Deepin桌面
struct ListSubItem {
    ListSubItem();
    ListSubItem(QString icon, QString text, QMetaMethod signal, QObject *plugin = nullptr, QString gsettingsName = QString())
    {
        this->itemIcon = icon;
        this->itemText = text;
        this->itemSignal = signal;
        this->pulgin = plugin;
        this->gsettingsName = gsettingsName;
    }
    QString itemIcon;
    QString itemText;
    QMetaMethod itemSignal;
    QObject *pulgin = nullptr;
    QString gsettingsName;
};

inline bool compareVersion(const QString &targetVersion, const QString &baseVersion)
{
    QStringList version1 = baseVersion.split(".");
    QStringList version2 = targetVersion.split(".");

    if (version1.size() != version2.size()) {
        return false;
    }

    for (int i = 0; i < version1.size(); ++i) {
        // 相等判断下一个子版本号
        if (version1[i] == version2[i])
            continue;

        // 转成整形比较
        return (version1[i].toInt() <= version2[i].toInt());
    }

    return true;
}

#endif // V20_DISPLAY_UTILS_H
