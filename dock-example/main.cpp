// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "docktestwidget.h"

#include <QApplication>
#include <QDesktopWidget>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DockTestWidget testPluginWidget;
    QDesktopWidget *deskdop = QApplication::desktop();
    testPluginWidget.move(100, deskdop->height() - 100);

    testPluginWidget.show();

    return app.exec();
}
