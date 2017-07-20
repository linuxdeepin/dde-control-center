#include "systeminfowidget.h"
#include "settingsgroup.h"
#include "titlevalueitem.h"
#include "logoitem.h"
#include "nextpagewidget.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QDebug>
#include <QApplication>

const QString systemCopyright()
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString oem_copyright = settings.value("system_info_vendor_name").toString();

    if (oem_copyright.isEmpty())
        return QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright © 2011-2017 Wuhan Deepin Technology Co., Ltd.");
    else
        return oem_copyright;
}

const QString systemLogo()
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString logo_path = settings.value("system_info_vendor_logo").toString();
    const QPixmap oem_logo(logo_path);

    if (oem_logo.isNull())
        return ":/systeminfo/themes/dark/icons/logo.png";
    else
        return logo_path;
}

namespace dcc{
namespace systeminfo{

SystemInfoWidget::SystemInfoWidget(SystemInfoModel* model)
    :ModuleWidget(),
      m_model(model)
{
    setObjectName("SysInfo");

    this->installEventFilter(parent());
    setTitle(tr("System Information"));

    SettingsGroup* infoGroup = new SettingsGroup();
    SettingsGroup* crGroup = new SettingsGroup();
    SettingsGroup* bootGroup = new SettingsGroup();

    LogoItem* logo = new LogoItem;
    logo->setDescription(systemCopyright());
    logo->setLogo(systemLogo());

    m_distroid = new TitleValueItem();
    m_distroid->setTitle(tr("Distribution:"));
    m_distroid->setValue(m_model->distroid());
    m_distroid->setVisible(false);

    m_distrover = new TitleValueItem();
    m_distrover->setTitle(tr("Distribution Release:"));
    m_distrover->setValue(m_model->distrover());
    m_distrover->setVisible(false);

    m_version = new TitleValueItem();
    m_version->setTitle(tr("Edition License:"));
    m_version->setValue(m_model->version());

    m_type = new TitleValueItem();
    m_type->setTitle(tr("Type:"));

    m_processor = new TitleValueItem();
    m_processor->setTitle(tr("Processor:"));
    m_processor->setValue(m_model->processor());

    m_memory = new TitleValueItem();
    m_memory->setTitle(tr("Memory:"));
    m_memory->setValue(m_model->memory());

    m_disk = new TitleValueItem();
    m_disk->setTitle(tr("Disk:"));
    m_disk->setValue(m_model->disk());

    infoGroup->appendItem(logo);
    infoGroup->appendItem(m_distroid);
    infoGroup->appendItem(m_distrover);
    infoGroup->appendItem(m_version);
    infoGroup->appendItem(m_type);
    infoGroup->appendItem(m_processor);
    infoGroup->appendItem(m_memory);
    infoGroup->appendItem(m_disk);

    m_copyright = new NextPageWidget();
    m_copyright->setTitle(tr("License"));

#ifndef DCC_DISABLE_GRUB
    m_boot = new NextPageWidget();
    m_boot->setTitle(tr("Boot Menu"));
#endif

    crGroup->appendItem(m_copyright);
#ifndef DCC_DISABLE_GRUB
    bootGroup->appendItem(m_boot);
#endif
    m_centralLayout->addWidget(infoGroup);
    m_centralLayout->addWidget(crGroup);
    m_centralLayout->addWidget(bootGroup);

    connect(m_copyright, SIGNAL(clicked()), this, SIGNAL(copyright()));
#ifndef DCC_DISABLE_GRUB
    connect(m_boot, SIGNAL(clicked()), this, SIGNAL(boot()));
#endif
    connect(m_model, SIGNAL(distroidChanged(QString)), this, SLOT(setDistroID(QString)));
    connect(m_model, SIGNAL(distroverChanged(QString)), this, SLOT(setDistroVer(QString)));
    connect(m_model, SIGNAL(versionChanged(QString)), this, SLOT(setEdition(QString)));
    connect(m_model, SIGNAL(typeChanged(QString)), this, SLOT(setType(QString)));
    connect(m_model, SIGNAL(processorChanged(QString)), this, SLOT(setProcessor(QString)));
    connect(m_model, SIGNAL(memoryChanged(QString)), this, SLOT(setMemory(QString)));
    connect(m_model, SIGNAL(diskChanged(QString)), this, SLOT(setDisk(QString)));

    setType(m_model->type());
}

void SystemInfoWidget::setDistroID(const QString &distroid)
{
    m_distroid->setValue(distroid);

    if (distroid == "Deepin") {
        m_distroid->setVisible(false);
        m_distrover->setVisible(false);
    }
    else {
        m_distroid->setVisible(true);
        m_distrover->setVisible(true);
    }
}

void SystemInfoWidget::setDistroVer(const QString &distrover)
{
    m_distrover->setValue(distrover);
}

void SystemInfoWidget::setEdition(const QString &edition)
{
    m_version->setValue(edition);
}

void SystemInfoWidget::setType(const QString &type)
{
    m_type->setValue(tr("%1Bit").arg(type));
}

void SystemInfoWidget::setProcessor(const QString &processor)
{
    m_processor->setValue(processor);
}

void SystemInfoWidget::setMemory(const QString &memory)
{
    m_memory->setValue(memory);
}

void SystemInfoWidget::setDisk(const QString &disk)
{
    m_disk->setValue(disk);
}

}
}
