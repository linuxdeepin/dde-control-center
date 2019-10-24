/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@gmail.com>
 *
 * Maintainer: justforlxz <justforlxz@gmail.com>
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

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QSaveFile>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <udisks2-qt5/ddiskdevice.h>
#include <udisks2-qt5/ddiskmanager.h>
#include <udisks2-qt5/dblockpartition.h>
#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>
#include <com_deepin_daemon_grub2.h>
#include <QScopedPointer>
#include <QLocale>

using AccountsInter = com::deepin::daemon::Accounts;
using UserInter = com::deepin::daemon::accounts::User;
using GrubInter = com::deepin::daemon::Grub2;

QStringList userList() {
    QScopedPointer<AccountsInter> accountsInter(new AccountsInter("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", QDBusConnection::systemBus()));
    accountsInter->setSync(true);
    const QStringList& dbusList { accountsInter->userList() };

    QStringList userList;
    for (const QString& dbus : dbusList) {
        QScopedPointer<UserInter> userInter(new UserInter("com.deepin.daemon.Accounts", dbus, QDBusConnection::systemBus()));
        userInter->setSync(true);
        userList << userInter->userName();
    }

    return userList;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Deepin recovery gui tools");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("conf", "Set Config ");

    QCommandLineOption fData("formatData", "formatData");
    parser.addOption(fData);

    parser.process(app);

    bool formatData = parser.isSet(fData);

    const QString &recoveryPath{ "/etc/deepin/system-recovery.conf" };
    QSettings settings(recoveryPath, QSettings::IniFormat);
    const QString UUID{ settings.value("UUID").toString() };
    if (UUID.isEmpty()) {
        qWarning() << "Cannot open " << recoveryPath;
        return -1;
    }

    QString mountPoint;
    const QStringList &devices = DDiskManager().blockDevices();
    for (const QString &path : devices) {
        QScopedPointer<DBlockDevice> device(DDiskManager::createBlockDevice(path));
        if (device->idUUID() == UUID) {
            device->unmount({});
            mountPoint = device->mount({});
            break;
        }
    }

    if (mountPoint.isEmpty()) {
        qWarning() << "mount point empty";
        return -1;
    }

    auto getUUID = [=] (const QString& path) -> QString {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return file.readAll().simplified();
        }

        return QString();
    };

    const QString& bootUUID{ getUUID(QString("%1/backup/boot.info").arg(mountPoint)) };
    const QString& rootUUID{ getUUID(QString("%1/backup/system.info").arg(mountPoint)) };
    const QString& dataUUID{ getUUID(QString("%1/backup/_dde_data.info").arg(mountPoint)) };

    QJsonObject fstabObj{ { "boot", QString("UUID:%1").arg(bootUUID) },
                          { "root", QString("UUID:%1").arg(rootUUID) },
                          { "data", "LABEL:_dde_data" } };

    if (QDir().exists("/sys/firmware/efi/")) {
        fstabObj["efi"] = QString("UUID:%1").arg(getUUID(QString("%1/backup/efi.info").arg(mountPoint)));
    }

    QJsonObject object{
        { "name", "restore" },
        { "message", "restore boot and root partition" },
        { "total", 7 },
        { "ask", true },
        { "locale", QLocale::system().name() },
        {
            "tasks",
            QJsonArray{
                QJsonObject{ { "message", "save user info" },
                             { "progress", true },
                             { "enable", false },
                             { "command", "save-user-info" },
                             { "args", QJsonArray{ userList().join(";") } },
                             { "env", QJsonObject{ { "source", QString("UUID:%1").arg(
                                                                   rootUUID) } } } },
                QJsonObject{ { "message", "starting restore boot partition" },
                             { "progress", true },
                             { "enable", true },
                             { "command", "restore-partitions" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(UUID),
                                                   "backup/boot.dim",
                                                   QString("UUID:%1").arg(bootUUID) } } },
                QJsonObject{ { "message", "starting restore root partition" },
                             { "progress", true },
                             { "enable", true },
                             { "command", "restore-partitions" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(UUID),
                                                   "backup/system.dim",
                                                   QString("UUID:%1").arg(rootUUID) } } },
                QJsonObject{ { "message", "restore user info" },
                             { "progress", true },
                             { "enable", false },
                             { "command", "restore-user-info" },
                             { "env", QJsonObject{ { "source", QString("UUID:%1").arg(
                                                                   rootUUID) } } } },
                QJsonObject{
                    { "message", "formating data partition" },
                    { "progress", false },
                    { "enable", formatData },
                    { "command", "refresh-data-partition" },
                    { "env", QJsonObject{ { "source", QString("UUID:%1").arg(dataUUID) },
                                          { "fstype", "ext4" } } } },
                QJsonObject{ { "message", "disable create user for init" },
                             { "enable", false },
                             { "command", "not-create-user" },
                             { "env", QJsonObject{ { "target", QString("UUID:%1").arg(
                                                                   rootUUID) } } } },
                QJsonObject{ { "message", "regenerate /etc/fstab" },
                             { "progress", false },
                             { "enable", true },
                             { "command", "generate-fstab" },
                             { "env", fstabObj } },
                QJsonObject{
                    { "message", "fix bootloader order" },
                    { "progress", false },
                    { "enable", true },
                    { "command", "fix-bootloader" },
                } },
        }
    };

    // 清理禁用的任务
    QJsonArray tasks = object["tasks"].toArray();
    for (auto it = tasks.begin(); it != tasks.end();) {
        QJsonObject obj{ it->toObject() };
        if (!obj["enable"].toBool()) {
            it = tasks.erase(it);
        }
        else {
            ++it;
        }
    }

    // 更新task的名字
    for (int i = 0; i != tasks.size(); ++i) {
        QJsonObject object = tasks[i].toObject();
        object["name"]     = QString("task-%1").arg(i + 1);
        tasks.replace(i, object);
    }

    object["tasks"] = tasks;
    object["total"] = tasks.size();

    QJsonDocument doc;
    doc.setObject(object);

    QSaveFile file(QString("%1/backup/recovery.json").arg(mountPoint));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(doc.toJson());
    file.commit();

    GrubInter* grubInter = new GrubInter("com.deepin.daemon.Grub2", "/com/deepin/daemon/Grub2", QDBusConnection::systemBus());
    grubInter->SetDefaultEntry("Deepin Recovery").waitForFinished();

    return 0;
}
