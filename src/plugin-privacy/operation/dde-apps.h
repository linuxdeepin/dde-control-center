// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include "dsglobal.h"

// from dde-shell->dde-apps

DS_BEGIN_NAMESPACE
class AppGroupManager {
public:
    enum Roles {
        GroupIdRole = Qt::UserRole + 1,
        GroupItemsPerPageRole,
        ExtendRole = 0x1000,
    };
};

class AppItemModel {
public:
    enum Roles {
        DesktopIdRole = AppGroupManager::ExtendRole,
        NameRole,
        IconNameRole,
        StartUpWMClassRole,
        NoDisplayRole,
        ActionsRole,
        DDECategoryRole,
        InstalledTimeRole,
        LastLaunchedTimeRole,
        LaunchedTimesRole,
        DockedRole,
        OnDesktopRole,
        AutoStartRole,
        AppTypeRole,
        XLingLongRole,
        IdRole,
        XCreatedByRole,
        ExecsRole,
    };
};
DS_END_NAMESPACE
