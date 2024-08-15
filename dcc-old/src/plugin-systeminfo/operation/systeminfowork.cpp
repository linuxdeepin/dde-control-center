//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systeminfowork.h"
#include "systeminfomodel.h"
#include "systeminfodbusproxy.h"
#include "window/utils.h"

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

QPair<QString, QString> SystemInfoWork::getGNULicenseText()
{
    if (!m_model->gnuLicense().has_value()) {
        m_model->setGnuLicense(DCC_LICENSE::loadLicenses());
    }
    return m_model->gnuLicense().value();
}

QString SystemInfoWork::getEndUserAgreementText()
{
    if (!m_model->endUserAgreement().has_value()) {
        if (DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
            m_model->setEndUserAgreement(DCC_LICENSE::getEulerEndUserAgreement());
        } else {
            if (m_model->endUserAgreementPath().has_value()) {
                m_model->setEndUserAgreement(DCC_LICENSE::getEndUserAgreement(m_model->endUserAgreementPath().value()));
            }
        }
    }
    return m_model->endUserAgreement().value();
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
