# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Sound module tests: Port and SoundDeviceData data objects.
dcc_add_unit_test(ut-sound
    ut_port.cpp
    ut_sounddevicedata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/soundDeviceData.cpp
)
target_include_directories(ut-sound PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)
# Port stores its id as QDBusObjectPath.
target_link_libraries(ut-sound PRIVATE ${QT_NS}::DBus)
