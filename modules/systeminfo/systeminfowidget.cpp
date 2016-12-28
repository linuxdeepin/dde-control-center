#include "systeminfowidget.h"
#include "settingsgroup.h"
#include "titlevalueitem.h"

#include <QVBoxLayout>

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

    LogoItem* logo = new LogoItem();
    logo->setDescription(tr("Copyright © 2011-2017 Wuhan Deepin Technology Co., Ltd."));

    m_version = new TitleValueItem();
    m_version->setTitle(tr("Edition:"));
    m_version->setValue(m_model->version());

    m_type = new TitleValueItem();
    m_type->setTitle(tr("Type:"));
    m_type->setValue(tr("%1Bit").arg(m_model->type()));

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
    infoGroup->appendItem(m_version);
    infoGroup->appendItem(m_type);
    infoGroup->appendItem(m_processor);
    infoGroup->appendItem(m_memory);
    infoGroup->appendItem(m_disk);

    m_copyright = new NextPageWidget();
    m_copyright->setTitle(tr("Protocol"));

    m_boot = new NextPageWidget();
    m_boot->setTitle(tr("Startup Menu"));

    crGroup->appendItem(m_copyright);
    bootGroup->appendItem(m_boot);

    m_centeralLayout->addWidget(infoGroup);
    m_centeralLayout->addWidget(m_copyright);
    m_centeralLayout->addWidget(m_boot);

    connect(m_copyright, SIGNAL(clicked()), this, SIGNAL(copyright()));
    connect(m_boot, SIGNAL(clicked()), this, SIGNAL(boot()));
    connect(m_model, SIGNAL(versionChanged(QString)), this, SLOT(setEditor(QString)));
    connect(m_model, SIGNAL(typeChanged(QString)), this, SLOT(setType(QString)));
    connect(m_model, SIGNAL(processorChanged(QString)), this, SLOT(setProcessor(QString)));
    connect(m_model, SIGNAL(memoryChanged(QString)), this, SLOT(setMemory(QString)));
    connect(m_model, SIGNAL(diskChanged(QString)), this, SLOT(setDisk(QString)));
}

void SystemInfoWidget::setEditor(const QString &edition)
{
    m_version->setValue(edition);
}

void SystemInfoWidget::setType(const QString &type)
{
    m_type->setValue(type);
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
