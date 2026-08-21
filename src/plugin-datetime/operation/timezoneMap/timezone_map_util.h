//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef INSTALLER_DELEGATES_TIMEZONE_MAP_UTIL_H
#define INSTALLER_DELEGATES_TIMEZONE_MAP_UTIL_H

#include "timezone.h"

namespace installer {

// Convert position of zone from polar coordinates to rectangular coordinates.
double ConvertLatitudeToY(double latitude);
double ConvertLongitudeToX(double longitude);

// Get a list of zone info whose distance to (x, y) is less than |threshold|
// in a world map with size (map_width, map_height).
ZoneInfoList GetNearestZones(const ZoneInfoList& total_zones, double threshold,
                             int x, int y, int map_width, int map_height);

}  // namespace installer

#endif  // INSTALLER_DELEGATES_TIMEZONE_MAP_UTIL_H
