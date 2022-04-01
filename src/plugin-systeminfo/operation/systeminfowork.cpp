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
#include "systeminfodbusproxy.h"

#include <DSysInfo>

DCORE_USE_NAMESPACE
namespace DCC_NAMESPACE{

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_systemInfDBusProxy(new SystemInfoDBusProxy(this))
{
    qRegisterMetaType<ActiveState>("ActiveState");
    connect(m_systemInfDBusProxy, &SystemInfoDBusProxy::StaticHostnameChanged, m_model, &SystemInfoModel::setHostName);
    connect(m_systemInfDBusProxy, &SystemInfoDBusProxy::AuthorizationStateChanged, m_model, [this] (const int state) {
        m_model->setLicenseState(static_cast<ActiveState>(state));
    });
}

void SystemInfoWork::activate()
{
    //获取主机名
    m_model->setHostName(m_systemInfDBusProxy->staticHostname());


    if (DSysInfo::isDeepin()) {
        m_model->setLicenseState(static_cast<ActiveState>(m_systemInfDBusProxy->authorizationState()));
    }

    m_model->setKernel(QSysInfo::kernelVersion());
    m_model->setProcessor(DSysInfo::cpuModelName());

    QString productName = QString("%1").arg(DSysInfo::uosSystemName());
    m_model->setProductName(productName);
    QString versionNumber = QString("%1").arg(DSysInfo::majorVersion());
    m_model->setVersionNumber(versionNumber);
    QString version;
    if (DSysInfo::uosType() == DSysInfo::UosServer || DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        version = QString("%1%2").arg(DSysInfo::minorVersion(), DSysInfo::uosEditionName());
    } else if (DSysInfo::isDeepin()) {
        version = QString("%1 (%2)").arg(DSysInfo::uosEditionName(), DSysInfo::minorVersion());
    } else {
        version = QString("%1 %2").arg(DSysInfo::productVersion(), DSysInfo::productTypeString());
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
    m_systemInfDBusProxy->Show();
}

void SystemInfoWork::onSetHostname(const QString &hostname)
{
    m_systemInfDBusProxy->setStaticHostname(hostname);
}

}
