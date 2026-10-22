// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "touchscreenmap.h"

void registerTouchscreenMapMetaType()
{
    qRegisterMetaType<TouchscreenMap>("TouchscreenMap");
    qDBusRegisterMetaType<TouchscreenMap>();
}
