//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systeminfomodel.h"

#include "math.h"

namespace DCC_NAMESPACE{

static QString formatCap(qulonglong cap, const int size = 1024, quint8 precision = 1)
{
    static QStringList type = {" B", " KB", " MB", " GB", " TB"};
    qulonglong lc = cap;
    double dc = cap;
    double ds = size;

    for(int p = 0; p < type.count(); ++p) {
        if (cap < pow(size, p + 1) || p == type.count() - 1) {
            if (!precision) {
#ifdef __sw_64__
                return QString::number(ceil(lc / pow(size, p))) + type[p];
#else
                return QString::number(round(lc / pow(size, p))) + type[p];
#endif
            }

            return QString::number(dc / pow(ds, p), 'f', precision) + type[p];
        }
    }

    return "";
}

SystemInfoModel::SystemInfoModel(QObject *parent)
    : QObject(parent)
    , m_type(64)
    , m_endUserAgreementText(std::nullopt)
    , m_gnuLicense(std::nullopt)
    , m_licenseState(ActiveState::Unauthorized)
{

}

void SystemInfoModel::setProductName(const QString& name)
{
    if(m_productName == name)
        return;

    m_productName = name;
    Q_EMIT productNameChanged(m_productName);
}
void SystemInfoModel::setVersionNumber(const QString& number)
{
    if(m_versionNumber == number)
        return;

    m_versionNumber = number;
    Q_EMIT versionNumberChanged(m_versionNumber);
}

void SystemInfoModel::setVersion(const QString &version)
{
    if(m_version == version)
        return;

    m_version = version;
    Q_EMIT versionChanged(m_version);
}

void SystemInfoModel::setType(qlonglong type)
{
    if(m_type == QString("%1").arg(type))
        return ;

    m_type = QString("%1").arg(type);
    typeChanged(m_type);
}

void SystemInfoModel::setProcessor(const QString &processor)
{
    if(m_processor == processor)
        return;

    m_processor = processor;
    processorChanged(processor);
}

void SystemInfoModel::setHostName(const QString &hostName)
{
    m_hostName = hostName;
    hostNameChanged(hostName);
}

void SystemInfoModel::setEndUserAgreement(const QString &text)
{
    m_endUserAgreementText = text;
}

void SystemInfoModel::setEndUserAgreementPath(const QString &path)
{
    m_endUserAgreementTextPath = path;
}

void SystemInfoModel::setGnuLicense(const QPair<QString, QString>& license)
{
    m_gnuLicense = license;
}

void SystemInfoModel::setMemory(qulonglong totalMemory, qulonglong installedMemory)
{
    QString mem_device_size = formatCap(installedMemory, 1024, 0);
    QString mem = formatCap(totalMemory);
    if(m_memory == mem)
        return ;

    m_memory = mem;
    m_memory = QString("%1 (%2)").arg(mem, tr("available"));
    memoryChanged(m_memory);
}


void SystemInfoModel::setKernel(const QString &kernel)
{
    if (m_kernel == kernel)
        return;

    m_kernel = kernel;
    kernelChanged(kernel);
}

void SystemInfoModel::setLicenseState(DCC_NAMESPACE::ActiveState state)
{
    if (m_licenseState != state) {
        m_licenseState = state;
        Q_EMIT licenseStateChanged(state);
    }
}

}
