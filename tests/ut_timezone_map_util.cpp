// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timezone_map_util.h"

#include <gtest/gtest.h>

#include <cmath>

// timezone_map_util.cpp 提供时区世界地图的坐标转换（经纬度 → 归一化
// 矩形坐标 x/y ∈ [0,1]）与「点击点最近时区」算法，被 datetime 插件
// datetimemodel.cpp 用于地图选时区。这些是纯函数/纯算法，无 Qt UI / D-Bus /
// 文件系统依赖，断言可完全确定。参考值由公式精确计算得到（见注释）。

namespace {

using installer::ConvertLatitudeToY;
using installer::ConvertLongitudeToX;
using installer::GetNearestZones;
using installer::ZoneInfo;
using installer::ZoneInfoList;

// tolerance for EXPECT_NEAR on the map-projection math
constexpr double kTol = 1e-4;

ZoneInfo makeZone(double latitude, double longitude, const QString &name = QStringLiteral("z"))
{
    ZoneInfo z;
    z.country = QStringLiteral("C");
    z.timezone = name;
    z.latitude = latitude;
    z.longitude = longitude;
    z.distance = 0.0;
    return z;
}

} // namespace

// ---- ConvertLongitudeToX ----

TEST(TimezoneMapUtil, ConvertLongitudeToXMapsKnownMeridians)
{
    // Formula: (180.0 + longitude) / 360.0 + (-6) / 180.0
    EXPECT_NEAR(ConvertLongitudeToX(-180.0), -0.0333333333, kTol);
    EXPECT_NEAR(ConvertLongitudeToX(0.0), 0.4666666667, kTol);
    EXPECT_NEAR(ConvertLongitudeToX(180.0), 0.9666666667, kTol);
}

TEST(TimezoneMapUtil, ConvertLongitudeToXIsMonotonicallyIncreasing)
{
    EXPECT_LT(ConvertLongitudeToX(-90.0), ConvertLongitudeToX(0.0));
    EXPECT_LT(ConvertLongitudeToX(0.0), ConvertLongitudeToX(90.0));
    EXPECT_LT(ConvertLongitudeToX(90.0), ConvertLongitudeToX(180.0));
}

TEST(TimezoneMapUtil, ConvertLongitudeToXIsLinear)
{
    // The mapping is affine in longitude (slope 1/360), so deltas are equal
    // for equal longitude increments.
    const double d180 = ConvertLongitudeToX(180.0) - ConvertLongitudeToX(0.0);
    const double d180b = ConvertLongitudeToX(0.0) - ConvertLongitudeToX(-180.0);
    EXPECT_NEAR(d180, d180b, kTol);
    EXPECT_NEAR(d180, 0.5, kTol); // 360/360 = 0.5 per 180 degrees
}

// ---- ConvertLatitudeToY ----

TEST(TimezoneMapUtil, ConvertLatitudeToYMapsKnownParallels)
{
    // Reference values from the closed-form Mercator-like projection:
    //   y(-59)=1.0, y(0)=0.6390, y(30)=0.4727, y(60)=0.2701, y(81)=0.0617
    EXPECT_NEAR(ConvertLatitudeToY(-59.0), 1.0, kTol);
    EXPECT_NEAR(ConvertLatitudeToY(0.0), 0.6390437968, kTol);
    EXPECT_NEAR(ConvertLatitudeToY(81.0), 0.0617411490, kTol);
}

TEST(TimezoneMapUtil, ConvertLatitudeToYIsMonotonicallyDecreasing)
{
    // Higher latitude → smaller normalized y (further from the bottom edge).
    EXPECT_GT(ConvertLatitudeToY(-59.0), ConvertLatitudeToY(0.0));
    EXPECT_GT(ConvertLatitudeToY(0.0), ConvertLatitudeToY(30.0));
    EXPECT_GT(ConvertLatitudeToY(60.0), ConvertLatitudeToY(81.0));
}

TEST(TimezoneMapUtil, ConvertLatitudeToYStaysInUnitRangeForSupportedLatitudes)
{
    // The function documents supported latitudes as [-59, 81].
    for (double lat = -59.0; lat <= 81.0; lat += 10.0) {
        const double y = ConvertLatitudeToY(lat);
        EXPECT_GE(y, 0.0);
        EXPECT_LE(y, 1.0);
    }
}

// ---- GetNearestZones ----

TEST(GetNearestZones, ReturnsZoneWithinThreshold)
{
    // One zone at longitude 0 (→ x≈0.4667) on a 1000px-wide map lands at
    // pixel x = 466. Clicking at x=470 with a generous threshold should
    // include it.
    ZoneInfoList zones;
    zones.append(makeZone(0.0, 0.0, QStringLiteral("z0")));

    const int mapWidth = 1000;
    const int mapHeight = 500;
    const int clickX = int(ConvertLongitudeToX(0.0) * mapWidth);
    const int clickY = int(ConvertLatitudeToY(0.0) * mapHeight);

    const ZoneInfoList result = GetNearestZones(zones, 1e6 /* huge threshold */,
                                                clickX, clickY, mapWidth, mapHeight);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first().timezone, QStringLiteral("z0"));
}

TEST(GetNearestZones, IncludesAllZonesWithinThresholdOrderedByInput)
{
    // With a large threshold every zone satisfies distance <= threshold, so
    // all of them are appended (in iteration order). The first appended is
    // the list's first element, not necessarily the globally nearest.
    ZoneInfoList zones;
    zones.append(makeZone(0.0, 10.0, QStringLiteral("near")));   // 10°E
    zones.append(makeZone(0.0, 170.0, QStringLiteral("far")));   // 170°E

    const int mapWidth = 1000;
    const int mapHeight = 500;
    // Click exactly at the projection of the "near" zone (10°E).
    const int clickX = int(ConvertLongitudeToX(10.0) * mapWidth);
    const int clickY = int(ConvertLatitudeToY(0.0) * mapHeight);

    const ZoneInfoList result = GetNearestZones(zones, 1e9, clickX, clickY, mapWidth, mapHeight);
    // Both zones qualify under the huge threshold.
    EXPECT_EQ(result.size(), 2);
    // Iteration order is preserved: "near" (index 0) was appended first.
    EXPECT_EQ(result.first().timezone, QStringLiteral("near"));
}

TEST(GetNearestZones, FallsBackToNearestZoneWhenNoneWithinThreshold)
{
    // No zone is within threshold → the function appends the single nearest
    // zone (nearest_zone_index branch, zones.isEmpty() == true).
    ZoneInfoList zones;
    zones.append(makeZone(0.0, 0.0, QStringLiteral("a")));
    zones.append(makeZone(0.0, 120.0, QStringLiteral("b")));

    const int mapWidth = 1000;
    const int mapHeight = 500;
    // Click near zone "a".
    const int clickX = int(ConvertLongitudeToX(0.0) * mapWidth);
    const int clickY = int(ConvertLatitudeToY(0.0) * mapHeight);

    // Negative threshold → no zone satisfies distance <= threshold (distance
    // is non-negative), so zones stays empty and the nearest-zone fallback
    // branch (zones.isEmpty()) appends total_zones.at(nearest_zone_index).
    const ZoneInfoList result = GetNearestZones(zones, -1.0, clickX, clickY, mapWidth, mapHeight);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result.first().timezone, QStringLiteral("a"));
}

TEST(GetNearestZones, EmptyInputReturnsEmptyList)
{
    ZoneInfoList empty;
    const ZoneInfoList result = GetNearestZones(empty, 64.0, 100, 100, 800, 400);
    EXPECT_TRUE(result.isEmpty());
}
