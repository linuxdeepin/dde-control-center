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
#include <QScopedPointer>
#include <QLocale>
#include <QTextStream>
#include <QDate>
#include <QDateTime>
#include <QSharedPointer>
#include <QProcess>

using AccountsInter = com::deepin::daemon::Accounts;
using UserInter = com::deepin::daemon::accounts::User;

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

enum class ActionType {
    Null,
    ManualBackup,
    SystemBackup,
    ManualRestore,
    SystemRestore,
};

static ActionType actionType = ActionType::Null;

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

    QCommandLineOption actionTypeOption("actionType", "action type", "type");
    parser.addOption(actionTypeOption);

    QCommandLineOption realtivePathOption("path", "realtive path", "directory");
    parser.addOption(realtivePathOption);

    parser.process(app);

    bool formatData = parser.isSet(fData);
    const QString& absolutePath = parser.value(realtivePathOption);

    const QMap<QString, ActionType> typeMap {
        {"manual_backup", ActionType::ManualBackup},
        {"system_backup", ActionType::SystemBackup},
        {"manual_restore", ActionType::ManualRestore},
        {"system_restore", ActionType::SystemRestore}
    };

    actionType = typeMap.value(parser.value(actionTypeOption), ActionType::Null);

    if (!parser.isSet(actionTypeOption) || actionType == ActionType::Null) {
        qWarning() << "not set Action Type";
        return -2;
    }

    const QString &recoveryPath{ "/etc/deepin/system-recovery.conf" };
    QSettings settings(recoveryPath, QSettings::IniFormat);
    const QString UUID{ settings.value("UUID").toString() };
    if (UUID.isEmpty()) {
        qWarning() << "Cannot open " << recoveryPath;
        return -1;
    }

    QString bootUUID;
    QString rootUUID;
    QString dataUUID;
    QString mountPoint;
    QString realtiveUUID;
    QString realtivePath;

    auto getUUID = [=] (const QString& path) -> QString {
        QProcess* findRealtive = new QProcess;
        findRealtive->setProgram("mount");
        findRealtive->start();
        findRealtive->waitForFinished();
        QTextStream steam(findRealtive->readAllStandardOutput());
        findRealtive->deleteLater();
        QString line;
        QMap<QString, QString> mountMap;
        while (steam.readLineInto(&line)) {
            const QStringList& list = line.split(" ");
            mountMap[list[2]] = list.first();
        }

        const QString& mountP = mountMap[path];
        if (!mountP.isEmpty()) {
            QProcess* p = new QProcess;
            p->setProgram("/sbin/blkid");
            p->setArguments({"-s", "UUID", "-o", "value", mountP});
            p->start();
            p->waitForFinished();
            return p->readAll().simplified();
        }

        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return file.readAll().simplified();
        }

        return QString();
    };

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

    dataUUID = getUUID(QString("%1/backup/_dde_data.info").arg(mountPoint));

    if (actionType == ActionType::SystemRestore) {
        bootUUID = getUUID(QString("%1/backup/boot.info").arg(mountPoint));
        rootUUID = getUUID(QString("%1/backup/system.info").arg(mountPoint));
    }

    if (actionType == ActionType::ManualBackup || actionType == ActionType::SystemBackup || actionType == ActionType::ManualRestore) {
        QProcess* findRealtive = new QProcess;
        findRealtive->setProgram("mount");
        findRealtive->setArguments({"-f"});
        findRealtive->start();
        findRealtive->waitForFinished();
        QTextStream steam(findRealtive->readAllStandardOutput());
        findRealtive->deleteLater();
        QString line;
        QStringList mountList;

        while (steam.readLineInto(&line)) {
            const QStringList& list = line.split(" ");
            mountList << list[2];
        }

        std::sort(mountList.begin(), mountList.end(), [=] (const QString& v1, const QString& v2) {
            return v1.length() > v2.length();
        });

        realtivePath = absolutePath;
        // FIXME(justforlxz): 这里特殊处理一下，因为/home和/data/home是bind关系
        if (realtivePath.startsWith("/home")) {
            realtivePath = "/data" + realtivePath;
        }

        for (const QString& mount : mountList) {
            if (QString(realtivePath).startsWith(mount)) {
                realtiveUUID = getUUID(mount);
                realtivePath.remove(0, mount.length());
                break;
            }
        }


        rootUUID = getUUID("/");
        bootUUID = getUUID("/boot");
    }

    QJsonObject fstabObj{ { "boot", QString("UUID:%1").arg(bootUUID) },
                          { "root", QString("UUID:%1").arg(rootUUID) },
                          { "data", "LABEL:_dde_data" } };

    if (QDir().exists("/sys/firmware/efi/")) {
        fstabObj["efi"] = QString("UUID:%1").arg(getUUID(QString("%1/backup/efi.info").arg(mountPoint)));
    }

    const QDateTime& currentTime = QDateTime::currentDateTime();
    const QString& timeDirectory = currentTime.toString("yyyy-MM-dd-hh-mm-ss");
    const QString& type = [=]() -> QString {
        if (actionType == ActionType::SystemRestore) {
            return "SystemRestore";
        }

        if (actionType == ActionType::ManualRestore) {
            return "ManualRestore";
        }

        if (actionType == ActionType::ManualBackup) {
            return "ManualBackup";
        }

        if (actionType == ActionType::SystemBackup) {
            return "SystemBackup";
        }
        Q_UNREACHABLE();
    }();

    QJsonObject object{
        { "name", "restore" },
        { "message", "restore boot and root partition" },
        { "total", 7 },
        { "ask", true },
        { "locale", QLocale::system().name() },
        { "type", type },
        { "tasks",
            QJsonArray{
                QJsonObject{
                    { "message", "fix bootloader order" },
                    { "progress", false },
                    { "enable", actionType == ActionType::ManualBackup || actionType == ActionType::SystemBackup},
                    { "command", "fix-bootloader" },
                    { "args", QJsonArray{ QString("UUID:%1").arg(bootUUID),
                                          QString("UUID:%1").arg(rootUUID),
                                          QString("UUID:%1").arg(dataUUID),
                                          [=]() -> QString {
#ifdef QT_DEBUG
                                              QSettings settings("/etc/default/grub", QSettings::IniFormat);
                                              return settings.value("GRUB_DEFAULT").toString();
#else
                                              return "0";
#endif
                                          }()
                                        }},
                },
                QJsonObject{ { "message", "save user info" },
                             { "progress", true },
                             { "enable", false },
                             { "command", "save-user-info" },
                             { "args", QJsonArray{ userList().join(";") } },
                             { "env", QJsonObject{ { "source", QString("UUID:%1").arg(
                                                                   rootUUID) } } } },
                QJsonObject{ { "message", "starting restore boot partition" },
                             { "progress", true },
                             { "enable", actionType == ActionType::SystemRestore },
                             { "command", "restore-partitions" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(UUID),
                                                   "backup/boot.dim",
                                                   QString("UUID:%1").arg(bootUUID) } } },
                QJsonObject{ { "message", "starting restore root partition" },
                             { "progress", true },
                             { "enable", actionType == ActionType::SystemRestore },
                             { "command", "restore-partitions" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(UUID),
                                                   "backup/system.dim",
                                                   QString("UUID:%1").arg(rootUUID) } } },
                QJsonObject{ { "message", "starting backup root & boot partition" },
                             { "progress", true },
                             { "enable", actionType == ActionType::SystemBackup },
                             { "command", "create-backup-image" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(bootUUID),
                                                   QString("UUID:%1").arg(realtiveUUID),
                                                   QString("%1/%2").arg(realtivePath).arg(timeDirectory),
                                                   "boot.dim",
                                                   "boot.md5"
                                                  } } },
                QJsonObject{ { "message", "starting backup root partition" },
                             { "progress", true },
                             { "enable", actionType == ActionType::ManualBackup || actionType == ActionType::SystemBackup},
                             { "command", "create-backup-image" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(rootUUID),
                                                   QString("UUID:%1").arg(realtiveUUID),
                                                   QString("%1/%2").arg(realtivePath).arg(timeDirectory),
                                                   "system.dim",
                                                   "system.md5"
                                                  } } },
                QJsonObject{ { "message", "starting restore root partition" },
                             { "progress", true },
                             { "enable", actionType == ActionType::ManualRestore },
                             { "command", "restore-partitions" },
                             { "args", QJsonArray{ QString("UUID:%1").arg(realtiveUUID),
                                                   QString("%1/system.dim").arg(realtivePath),
                                                   QString("UUID:%1").arg(rootUUID)
                                                  } } },
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
                             { "enable", false },
                             { "command", "generate-fstab" },
                             { "env", fstabObj } },
            }
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

    QSharedPointer<QProcess> process(new QProcess);

    if (QFile::exists("/usr/lib/deepin-recovery/prepare_recovery")) {
        process->start("pkexec", {"/usr/lib/deepin-recovery/prepare_recovery"});
        process->waitForFinished(-1);

        if (process->exitCode() != 0) {
            return -1;
        }
    }

    return 0;
}
