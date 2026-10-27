# SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Bluetooth module test: BluetoothDevice data object.
dcc_add_unit_test(ut-bluetooth
    ut_bluetoothdevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../operation/bluetoothdevice.cpp
)
target_include_directories(ut-bluetooth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../operation)
