//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
        QString productName = QString("%1").arg(DSysInfo::uosSystemName());
        m_model->setProductName(productName);
        QString versionNumber = QString("%1").arg(DSysInfo::majorVersion());
        m_model->setVersionNumber(versionNumber);
    }
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

    m_model->setKernel(QSysInfo::kernelVersion());
    m_model->setProcessor(DSysInfo::cpuModelName());
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
    m_systemInfDBusProxy->setStaticHostname(hostname, this, SLOT(onSetHostnameFinish()), SLOT(onSetHostnameFinish()));
}

void SystemInfoWork::onSetHostnameFinish()
{
    m_model->setHostName(m_systemInfDBusProxy->staticHostname());
}

}
