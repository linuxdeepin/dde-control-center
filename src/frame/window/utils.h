/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef V20_DISPLAY_UTILS_H
#define V20_DISPLAY_UTILS_H

#include "interface/namespace.h"

#include <QMargins>
#include <QSize>
#include <QVariant>
#include <QSettings>
#include <DSysInfo>

DCORE_USE_NAMESPACE

Q_DECLARE_METATYPE(QMargins)

namespace DCC_NAMESPACE {

const int FRAME_WIDTH = 408;
const int NAVBAR_WIDTH = 56;
const QMargins ThirdPageContentsMargins(0, 0, 0, 0);
const QMargins ScrollAreaMargins(0, 0, 0, 0);
const QMargins ListViweItemMargin(10, 8, 10, 8);
const QVariant VListViewItemMargin = QVariant::fromValue(ListViweItemMargin);

const QSize ListViweItemIconSize(84,84);
const QSize ListViweItemSize(170,168);
const QSize ListViweItemIconSize_ListMode(42,42);
const QSize ListViweItemSize_ListMode(168,48);

const qint32 ActionIconSize=30;//大图标角标大小
const qint32 ActionListSize=26;//list图标角标大小
const qint32 ListViweRightSubscriptItemDis=qMax(ActionIconSize,ActionListSize)+18;

const int List_Interval = 10;
const QMargins ArrowEnterClickMargin(8, 8, 8, 8);

const QString SystemTypeName = DSysInfo::deepinTypeDisplayName(QLocale::c());

const DSysInfo::DeepinType DeepinType = DSysInfo::deepinType();
const bool IsServerSystem = (DSysInfo::DeepinServer == DeepinType);
const bool IsDesktopSystem = DSysInfo::isCommunityEdition();
const bool IsProfessionalSystem = (DSysInfo::DeepinProfessional == DeepinType);

}

#endif // V20_DISPLAY_UTILS_H
