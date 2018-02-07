/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "sysinfo/timezone.h"

#include "third_party/googletest/include/gtest/gtest.h"

namespace installer {
namespace {

TEST(TimezoneTest, GetZoneInfoList) {
  const ZoneInfoList list = GetZoneInfoList();
  EXPECT_FALSE(list.isEmpty());
  const int index = GetZoneInfoByZone(list, "Asia/Shanghai");
  EXPECT_TRUE(index > -1);
  const ZoneInfo info = list.at(index);
  EXPECT_EQ(info.latitude, 31.14);
  EXPECT_EQ(info.longitude, 121.28);
}

TEST(TimezoneTest, GetCurrentTimezone) {
  const QString current_timezone = GetCurrentTimezone();
  EXPECT_FALSE(current_timezone.isEmpty());
}

TEST(TimezoneTest, GetTimezoneAliasMap) {
  const TimezoneAliasMap map = GetTimezoneAliasMap();
  EXPECT_GT(map.keys().length(), 0);
  EXPECT_TRUE(map.contains("Asia/Chongqing"));
  EXPECT_EQ(map.value("Asia/Chongqing"), "Asia/Shanghai");
}

TEST(TimezoneTest, GetTimezoneName) {
  EXPECT_EQ(GetTimezoneName("Asia/Shanghai"), "Shanghai");
}

TEST(TimezoneTest, GetTimezoneOffset) {
  const TimezoneOffset shanghai = GetTimezoneOffset("Asia/Shanghai");
  EXPECT_TRUE(shanghai.name == "CST");
  EXPECT_EQ(shanghai.seconds, 28800);

  const TimezoneOffset lima = GetTimezoneOffset("America/Lima");
  EXPECT_TRUE(lima.name == "PET");
  EXPECT_EQ(lima.seconds, -18000);
}

TEST(TimezoneTest, IsValidTimezone) {
  EXPECT_TRUE(IsValidTimezone("Asia/Shanghai"));
  EXPECT_TRUE(IsValidTimezone("Etc/UTC"));
  EXPECT_FALSE(IsValidTimezone("Asia/Beijing"));
}

}  // namespace
}  // namespace installer