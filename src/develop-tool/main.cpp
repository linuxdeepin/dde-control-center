// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
