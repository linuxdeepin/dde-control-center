// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>

namespace DccWallpaperThumbnail {

QString cachePath(const QString &videoPath);
QString generate(const QString &videoPath);

}
