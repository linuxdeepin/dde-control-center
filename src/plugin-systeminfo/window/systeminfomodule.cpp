/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include <QApplication>

#include "systeminfomodule.h"
#include "nativeinfowidget.h"
#include "versionprotocolwidget.h"
#include "userlicensewidget.h"
#include "privacypolicywidget.h"

#include "src/plugin-systeminfo/operation/systeminfowork.h"
#include "src/plugin-systeminfo/operation/systeminfomodel.h"

using namespace DCC_NAMESPACE;

SystemInfoModule::SystemInfoModule(QObject *parent)
    : ModuleObject(parent)
    , m_work(nullptr)
    , m_model(nullptr)
{
    if (m_model) {
        delete m_model;
    }
    m_model = new SystemInfoModel();
    m_work = new SystemInfoWork(m_model);
    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

SystemInfoModule::~SystemInfoModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void SystemInfoModule::active()
{
    m_work->activate();
}

QString SystemInfoPlugin::name() const
{
    return QStringLiteral("systeminfo");
}

ModuleObject *SystemInfoPlugin::module()
{
    //一级菜单--系统信息
    SystemInfoModule *moduleInterface = new SystemInfoModule(this);
    moduleInterface->setName("systeminfo");
    moduleInterface->setDisplayName(tr("System Info"));
    moduleInterface->setDescription(tr("System Info"));
    moduleInterface->setIcon(QIcon::fromTheme("dcc_nav_systeminfo"));
    moduleInterface->setChildType(ModuleObject::HList);

    //二级菜单--关于本机
    ModuleObject *moduleAboutPc = new ModuleObject("aboutThisPc", tr("About This PC"), QIcon::fromTheme("dcc_on_sel"), this);
    moduleAboutPc->setChildType(ModuleObject::Page);
    NativeInfoModule *nativeInfoModule = new NativeInfoModule(moduleInterface->model(), moduleInterface->work(), moduleAboutPc);
    moduleAboutPc->appendChild(nativeInfoModule);
    moduleInterface->appendChild(moduleAboutPc);


    //二级菜单--协议与隐私政策
    ModuleObject *moduleAgreement = new ModuleObject("agreement", tr("Agreement and privacy policy"), QIcon::fromTheme("dcc_version"), this);
    moduleAgreement->setChildType(ModuleObject::VList);

    //三级菜单--协议与隐私政策-版本协议
    ModuleObject *moduleEdition = new ModuleObject("editionLicense", tr("Edition License"), QIcon::fromTheme("dcc_version"), moduleAgreement);
    moduleEdition->setChildType(ModuleObject::Page);
    VersionProtocolModule *versionProtocolModule = new VersionProtocolModule(moduleEdition);
    moduleEdition->appendChild(versionProtocolModule);
    moduleAgreement->appendChild(moduleEdition);

    //三级菜单--协议与隐私政策-最终用户许可协议
    ModuleObject *moduleUserAgreement = new ModuleObject("endUserLicenseAgreement", tr("End User License Agreement"), QIcon::fromTheme("dcc_protocol"), moduleAgreement);
    moduleUserAgreement->setChildType(ModuleObject::Page);
    UserLicenseModule *userLicenseModule = new UserLicenseModule(moduleUserAgreement);
    moduleUserAgreement->appendChild(userLicenseModule);
    moduleAgreement->appendChild(moduleUserAgreement);

    //三级菜单--协议与隐私政策-隐私政策
    ModuleObject *modulePolicy = new ModuleObject("privacyPolicy", tr("Privacy Policy"), QIcon::fromTheme("dcc_privacy_policy"), moduleAgreement);
    modulePolicy->setChildType(ModuleObject::Page);
    PrivacyPolicyModule *privacyPolicyModule = new PrivacyPolicyModule(modulePolicy);
    modulePolicy->appendChild(privacyPolicyModule);
    moduleAgreement->appendChild(modulePolicy);

    moduleInterface->appendChild(moduleAgreement);

    return moduleInterface;
}

int SystemInfoPlugin::location() const
{
    return 21;
}

NativeInfoModule::NativeInfoModule(SystemInfoModel *model, SystemInfoWork *work, QObject *parent)
    : ModuleObject(parent)
{
    m_model = model;
    m_work = work;
}

QWidget *NativeInfoModule::page()
{
    NativeInfoWidget *w = new NativeInfoWidget(this->m_model);
    connect(w, &NativeInfoWidget::clickedActivator, this->m_work, &SystemInfoWork::showActivatorDialog);
    connect(w, &NativeInfoWidget::requestSetHostname, this->m_work, &SystemInfoWork::onSetHostname);
    return w;
}

QWidget *PrivacyPolicyModule::page()
{
    return new PrivacyPolicyWidget();
}

QWidget *UserLicenseModule::page()
{
    return new UserLicenseWidget();
}

QWidget *VersionProtocolModule::page()
{
    return new VersionProtocolWidget();
}
