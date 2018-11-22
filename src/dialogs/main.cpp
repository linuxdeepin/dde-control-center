/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "reminderdialog.h"

#include <DApplication>
#include <QTranslator>
#include <DDBusSender>
#include <QDBusConnection>

#define Service "com.deepin.dialogs.RebootReminder"
#define Path "/com/deepin/dialogs/RebootReminder"
#define Interface "com.deepin.dialogs.RebootReminder"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    translator.load("/usr/share/dde-control-center/translations/dialogs_" + QLocale::system().name());
    a.installTranslator(&translator);

    bool res = QDBusConnection::sessionBus().registerService(Service);
    if (res) {
        Manager *manager = new Manager;

        QDBusConnection::sessionBus().registerObject(Path, Interface, manager, QDBusConnection::ExportAllSlots);

        manager->start();

        return a.exec();

    } else {
        qWarning() << "there's an instance running, invoking Show method on it.";

        DDBusSender()
                .service("com.deepin.dialogs.RebootReminder")
                .interface("com.deepin.dialogs.RebootReminder")
                .path("/com/deepin/dialogs/RebootReminder")
                .method("Show")
                .call();
    }
}
