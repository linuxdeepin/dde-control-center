/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "nativeinfowidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/titlevalueitem.h"
#include "modules/systeminfo/logoitem.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QSettings>
#include <QIcon>
#include <QScrollArea>
#include <QScroller>
#include <QDebug>
#include <DSysInfo>

using namespace dcc::widgets;
using namespace dcc::systeminfo;
#define GSETTINGS_CONTENS_SERVER "iscontens-server"

DCORE_USE_NAMESPACE
namespace DCC_NAMESPACE {
namespace systeminfo {

NativeInfoWidget::NativeInfoWidget(SystemInfoModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(model)
    , m_mainLayout(new QVBoxLayout)
    , isContensServers(false)
{
    initWidget();
}

void NativeInfoWidget::initWidget()
{
    QWidget *container = new dcc::widgets::TranslucentFrame;
    container->setLayout(m_mainLayout);
    setContent(container);

    SettingsGroup *infoGroup = new SettingsGroup();
    SettingsGroup *logoGroup = new SettingsGroup();
    logoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    infoGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    LogoItem *logo = new LogoItem;
    logo->setDescription(true); //显示文字描述
    logo->setDescription(systemCopyright());//LogoItem构造函数: set the discription visible=false
    logo->setLogo(DSysInfo::distributionOrgLogo(DSysInfo::Distribution, DSysInfo::Normal));

    if (QGSettings::isSchemaInstalled("com.deepin.dde.control-versiontype")) {
        m_moduleActive = new QGSettings("com.deepin.dde.control-versiontype", QByteArray(), this);
        isContensServers =  m_moduleActive->get(GSETTINGS_CONTENS_SERVER).toBool();
    }

    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer ||
            (DSysInfo::uosType() == DSysInfo::UosType::UosDesktop)) {
        m_productName= new TitleValueItem();
        //~ contents_path /systeminfo/About This PC
        m_productName->setTitle(tr("OS Name:"));
        m_productName->setValue(m_model->productName());

        m_versionNumber = new TitleValueItem();
        //~ contents_path /systeminfo/About This PC
        m_versionNumber->setTitle(tr("Version:"));
        m_versionNumber->setValue(m_model->versionNumber());
    }
    m_version = new TitleValueItem();
    //~ contents_path /systeminfo/About This PC
    m_version->setTitle(tr("Edition:"));
    m_version->setValue(m_model->version());
    m_type = new TitleValueItem();
    //~ contents_path /systeminfo/About This PC
    m_type->setTitle(tr("Type:"));

#ifndef DISABLE_ACTIVATOR
    m_authorized = new TitleAuthorizedItem();
    m_authorized->setTitle(tr("Authorization") + ':');
    m_authorized->setValue(tr("To be activated"));
    m_authorized->setValueForegroundRole(QColor(255, 0, 0));
    m_authorized->setButtonText(tr("Activate"));
    if (isContensServers) {
        m_authorized->setVisable(false);
    } else {
        m_authorized->setVisable(true);
    }
#endif

    m_kernel = new TitleValueItem();
    //~ contents_path /systeminfo/About This PC
    m_kernel->setTitle(tr("Kernel:"));
    m_kernel->setValue(m_model->kernel());

    m_processor = new TitleValueItem();
    //~ contents_path /systeminfo/About This PC
    m_processor->setTitle(tr("Processor:"));
    m_processor->setValue(m_model->processor());

    m_memory = new TitleValueItem();
    //~ contents_path /systeminfo/About This PC
    m_memory->setTitle(tr("Memory:"));
    m_memory->setValue(m_model->memory());

    logoGroup->appendItem(logo);
    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer ||
            (DSysInfo::uosType() == DSysInfo::UosType::UosDesktop)) {
        infoGroup->appendItem(m_productName);
        infoGroup->appendItem(m_versionNumber);
    }
    infoGroup->appendItem(m_version);
    infoGroup->appendItem(m_type);
    infoGroup->appendItem(m_authorized);
    infoGroup->appendItem(m_kernel);
    infoGroup->appendItem(m_processor);
    infoGroup->appendItem(m_memory);

    infoGroup->setSpacing(10);


    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);
    m_mainLayout->addWidget(logoGroup);
    m_mainLayout->addWidget(infoGroup);
    m_mainLayout->addStretch(10);
    setLayout(m_mainLayout);

    QScroller::grabGesture(this->window(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(this->window());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    connect(m_model, &SystemInfoModel::productNameChanged, this, &NativeInfoWidget::setProductName);
    connect(m_model, &SystemInfoModel::versionNumberChanged, this, &NativeInfoWidget::setVersionNumber);
    connect(m_model, &SystemInfoModel::versionChanged, this, &NativeInfoWidget::setEdition);
    connect(m_model, &SystemInfoModel::typeChanged, this, &NativeInfoWidget::setType);
    connect(m_model, &SystemInfoModel::processorChanged, this, &NativeInfoWidget::setProcessor);
    connect(m_model, &SystemInfoModel::memoryChanged, this, &NativeInfoWidget::setMemory);

#ifndef DISABLE_ACTIVATOR
    //传递button的点击信号
    connect(m_authorized, &TitleAuthorizedItem::clicked, this, &NativeInfoWidget::clickedActivator);
    connect(m_model, &SystemInfoModel::licenseStateChanged, this, &NativeInfoWidget::setLicenseState);
#endif

    setType(m_model->type());

#ifndef DISABLE_ACTIVATOR
    setLicenseState(m_model->licenseState());
#endif
}

void NativeInfoWidget::setProductName(const QString &edition)
{
    m_productName->setValue(edition);
}

void NativeInfoWidget::setVersionNumber(const QString &type)
{
    m_versionNumber->setValue(type);
}

void NativeInfoWidget::setEdition(const QString &edition)
{
    m_version->setValue(edition);
}

void NativeInfoWidget::setType(const QString &type)
{
    m_type->setValue(tr("%1-bit").arg(type));
}

void NativeInfoWidget::setProcessor(const QString &processor)
{
    m_processor->setValue(processor);
}

void NativeInfoWidget::setMemory(const QString &memory)
{
    m_memory->setValue(memory);
}

#ifndef DISABLE_ACTIVATOR
void NativeInfoWidget::setLicenseState(ActiveState state)
{
    if (state == Authorized) {
        m_authorized->setValue(tr("Activated"));
        m_authorized->setValueForegroundRole(QColor(21, 187, 24));
        m_authorized->setButtonText(tr("View"));
    } else if (state == Unauthorized) {
        m_authorized->setValue(tr("To be activated"));
        m_authorized->setValueForegroundRole(QColor(255, 87, 54));
        m_authorized->setButtonText(tr("Activate"));
    } else if (state == AuthorizedLapse) {
        m_authorized->setValue(tr("Expired"));
        m_authorized->setValueForegroundRole(QColor(255, 87, 54));
        m_authorized->setButtonText(tr("View"));
    } else if (state == TrialAuthorized) {
        m_authorized->setValue(tr("In trial period"));
        m_authorized->setValueForegroundRole(QColor(255, 170, 0));
        m_authorized->setButtonText(tr("Activate"));
    } else if (state == TrialExpired) {
        m_authorized->setValue(tr("Trial expired"));
        m_authorized->setValueForegroundRole(QColor(255, 87, 54));
        m_authorized->setButtonText(tr("Activate"));
    }
}
#endif

const QString NativeInfoWidget::systemCopyright() const
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString oem_copyright = settings.value("system_info_vendor_name").toString().toLatin1();

    if (oem_copyright.isEmpty()) {
        if (DCC_NAMESPACE::IsDesktopSystem)
            return QString(QApplication::translate("dcc::systeminfo::SystemInfoWidget", "www.deepin.org")).arg(2019);
        else
            return QString(QApplication::translate("dcc::systeminfo::SystemInfoWidget", "Copyright© 2019-2020 UnionTech Software Technology Co., LTD")).arg(2019);
    } else {
        return oem_copyright;
    }
}

const QString NativeInfoWidget::systemLogo() const
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString logo_path = settings.value("system_info_vendor_logo").toString();
    const QPixmap oem_logo(logo_path);

    if (oem_logo.isNull()) {
        return ":/systeminfo/themes/dark/icons/logo.png";
    } else {
        return logo_path;
    }
}

}
}
