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
#include <DDBusSender>

#include <QDBusConnection>

#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (0 != getuid()) {
        return -1;
    }

    auto inter = new QDBusInterface("com.deepin.sync.Helper",
                                                 "/com/deepin/sync/Helper",
                                                 "com.deepin.sync.Helper",
                                                 QDBusConnection::systemBus(), nullptr);

    QByteArray arr;
    arr.append(1);
    auto msg = inter->call("EnableDeveloperMode", arr);
    inter->deleteLater();
    if (msg.type() == QDBusMessage::ErrorMessage) {
        return -1;
    }

    std::cout << "enable develop success" << std::endl;
    return 0;
}
