//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "timezone_map_util.h"

#include <math.h>

namespace installer {

namespace {

// From gnome-control-center.
double radians(double degrees) {
  return (degrees / 360.0) * M_PI * 2;
}

}  // namespace

double ConvertLatitudeToY(double latitude) {
  const double bottom_lat = -59;
  const double top_lat = 81;
  const double full_range = 4.6068250867599998;
  double top_per, y, top_offset, map_range;

  top_per = top_lat / 180.0;
  y = 1.25 * log(tan(M_PI_4 + 0.4 * radians(latitude)));
  top_offset = full_range * top_per;
  map_range = fabs(1.25 * log(tan(M_PI_4 + 0.4 * radians(bottom_lat))) -
                   top_offset);
  y = fabs(y - top_offset);
  y = y / map_range;
  return y;
}

double ConvertLongitudeToX(double longitude) {
  const double xdeg_offset = -6;
  return ((180.0 + longitude) / 360.0 + xdeg_offset / 180.0);
}

ZoneInfoList GetNearestZones(const ZoneInfoList& total_zones, double threshold,
                             int x, int y, int map_width, int map_height) {
  ZoneInfoList zones;
  double minimum_distance = map_width * map_width + map_height * map_height;
  int nearest_zone_index = -1;
  for (int index = 0; index < total_zones.length(); index++) {
    const ZoneInfo& zone = total_zones.at(index);
    const double point_x = ConvertLongitudeToX(zone.longitude) * map_width;
    const double point_y = ConvertLatitudeToY(zone.latitude) * map_height;
    const double dx = point_x - x;
    const double dy = point_y - y;
    const double distance = dx * dx + dy * dy;
    if (distance < minimum_distance) {
      minimum_distance = distance;
      nearest_zone_index = index;
    }
    if (distance <= threshold) {
      zones.append(zone);
    }
  }

  // Get the nearest zone.
  if (zones.isEmpty()) {
    zones.append(total_zones.at(nearest_zone_index));
  }

  return zones;
}

}  // namespace installer
