//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <QApplication>
#include <gtest/gtest.h>
#ifdef QT_DEBUG
#ifdef USE_ASAN
#include <sanitizer/asan_interface.h>
#endif
#endif
int main(int argc, char **argv)
{
    setenv("QT_QPA_PLATFORM", "offscreen", 1);
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
#ifdef QT_DEBUG
#ifdef USE_ASAN
    __sanitizer_set_report_path("dccwidgets_asan.log");
#endif
#endif
    return ret;
}
