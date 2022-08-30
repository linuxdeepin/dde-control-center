// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QMetaType>

namespace dcc {
    namespace cloudsync {
        enum SyncType : int {
            Network,
            Sound,
            Mouse,
            Update,
            Dock,
            Launcher,
            Wallpaper,
            Theme,
            Power,
            Corner
        };

        enum SyncState : int {
            Succeed,
            Syncing,
            Failed,
        };
    }
}

Q_DECLARE_METATYPE(dcc::cloudsync::SyncType);
Q_DECLARE_METATYPE(dcc::cloudsync::SyncState);
