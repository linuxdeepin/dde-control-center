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
 *             Tianlu Shao <shaotianlu@uniontech.com>
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

#include "systeminfowork.h"
#include "systeminfomodel.h"

#include <DSysInfo>

#include <QApplication>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusArgument>
#include <QDBusReply>

DCORE_USE_NAMESPACE
namespace DCC_NAMESPACE{

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    :QObject(parent),
      m_model(model)
{
    //获取主机名
    QDBusInterface Interface("org.freedesktop.hostname1",
                                 "/org/freedesktop/hostname1",
                                "org.freedesktop.DBus.Properties",
                                QDBusConnection::systemBus());

    QDBusMessage reply = Interface.call("Get","org.freedesktop.hostname1","StaticHostname");
    QList<QVariant> outArgs = reply.arguments();
    QString hostName = outArgs.at(0).value<QDBusVariant>().variant().toString();
    m_model->setHostName(hostName);


    if (DSysInfo::isDeepin()) {
        QDBusConnection::systemBus().connect("com.deepin.license", "/com/deepin/license/Info",
                                             "com.deepin.license.Info", "LicenseStateChange",
                                             this, SLOT(licenseStateChangeSlot()));
        licenseStateChangeSlot();
    }

    //监控dbus上的属性改变信号
    QDBusConnection::systemBus().connect("org.freedesktop.hostname1",
                                          "/org/freedesktop/hostname1",
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(handleDbusSignal(QDBusMessage)));

    connect(m_model,&SystemInfoModel::setHostNameChanged, this, &SystemInfoWork::onSetHostname);

    QProcess process;
    process.start("uname -r");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    int idx = output.indexOf('\n');
    if ( -1 != idx) {
        output.remove(idx, 1);
    }
    m_model->setKernel(output);
    m_model->setProcessor(DSysInfo::cpuModelName());
}

void SystemInfoWork::handleDbusSignal(QDBusMessage meg)
{
    QList<QVariant> arguments = meg.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = meg.arguments().at(0).toString();
    if (interfaceName == "org.freedesktop.hostname1") {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "StaticHostname") {
                QString hostName = changedProps.value(keys.at(i)).toString();
                m_model->setHostName(hostName);
            }
        }
    }
}

void SystemInfoWork::onSetHostname(QString hostname)
{
    QDBusInterface Interface("org.freedesktop.hostname1",
                                 "/org/freedesktop/hostname1",
                                "org.freedesktop.hostname1",
                                QDBusConnection::systemBus());

     QDBusPendingCall async = Interface.asyncCall("SetStaticHostname", hostname, true);
     QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
     QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher* watcher){
        watcher->deleteLater();
     });
}

void SystemInfoWork::activate()
{
    qRegisterMetaType<ActiveState>("ActiveState");

    QString productName = QString("%1").arg(DSysInfo::uosSystemName());
    m_model->setProductName(productName);
    QString versionNumber = QString("%1").arg(DSysInfo::majorVersion());
    m_model->setVersionNumber(versionNumber);
    QString version;
    if (DSysInfo::uosType() == DSysInfo::UosServer || DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        version = QString("%1%2").arg(DSysInfo::minorVersion())
                                  .arg(DSysInfo::uosEditionName());
    } else if (DSysInfo::isDeepin()) {
        version = QString("%1 (%2)").arg(DSysInfo::uosEditionName())
                                  .arg(DSysInfo::minorVersion());
    } else {
        version = QString("%1 %2").arg(DSysInfo::productVersion())
                                  .arg(DSysInfo::productTypeString());
    }

    m_model->setVersion(version);
    m_model->setType(QSysInfo::WordSize);

   m_model->setMemory(static_cast<qulonglong>(DSysInfo::memoryTotalSize()), static_cast<qulonglong>(DSysInfo::memoryInstalledSize()));
}

void SystemInfoWork::deactivate()
{

}

void SystemInfoWork::showActivatorDialog()
{
    qDebug() << "SASASASASASA";
    QDBusInterface activator("com.deepin.license.activator",
                             "/com/deepin/license/activator",
                             "com.deepin.license.activator",
                             QDBusConnection::sessionBus());
    activator.call(QDBus::AutoDetect, "Show");
}

void SystemInfoWork::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher,
            &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &SystemInfoWork::getLicenseState);
    watcher->setFuture(future);
}

#ifndef DISABLE_ACTIVATOR
void SystemInfoWork::getLicenseState()
{
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qDebug() << "com.deepin.license error ," << licenseInfo.lastError().name();
        return;
    }

    ActiveState reply = licenseInfo.property("AuthorizationState").value<ActiveState>();
    qDebug() << "authorize result:" << reply;
    m_model->setLicenseState(reply);
}
#endif

}
