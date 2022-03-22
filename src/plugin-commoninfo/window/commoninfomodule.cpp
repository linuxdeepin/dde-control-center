/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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

#include "commoninfomodule.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "src/plugin-commoninfo/operation/commoninfowork.h"

#include "bootwidget.h"
#include "userexperienceprogramwidget.h"
#include "developermodewidget.h"

#include <DSysInfo>
#include <QApplication>

DCC_USE_NAMESPACE
DCORE_USE_NAMESPACE

CommonInfoModule::CommonInfoModule(QObject *parent)
    : ModuleObject(parent)
    , m_worker(nullptr)
    , m_model(nullptr)
{
    m_model = new CommonInfoModel();
    m_worker = new CommonInfoWork(m_model);

    m_worker->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

CommonInfoModule::~CommonInfoModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

QString CommonInfoPlugin::name() const
{
    return QStringLiteral("commoninfo");
}

ModuleObject *CommonInfoPlugin::module()
{   
    //一级菜单--通用设置
    CommonInfoModule *moduleInterface = new CommonInfoModule(this);
    ModuleData *dataRoot = new ModuleData(this);
    dataRoot->Name = tr("commoninfo");
    dataRoot->DisplayName = tr("General Settings");
    dataRoot->Description = tr("通用设置");
    dataRoot->Icon = QIcon::fromTheme("dcc_nav_commoninfo");
    moduleInterface->setModuleData(dataRoot);
    moduleInterface->setChildType(ModuleObject::ChildType::HList);

    //二级菜单--启动菜单
    ModuleObject *moduleBootMenu = new ModuleObject(tr("Boot Menu"), tr("Boot Menu"), this);
    moduleBootMenu->setChildType(ModuleObject::ChildType::Page);
    BootModule *bootModule = new BootModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleBootMenu->appendChild(bootModule);
    moduleInterface->appendChild(moduleBootMenu);

    //二级菜单--开发者模式
    ModuleObject *moduleDeveloperMode = new ModuleObject(tr("Developer Mode"), tr("Developer Mode"), this);
    moduleDeveloperMode->setChildType(ModuleObject::ChildType::Page);
    DeveloperModeModule *developerModeModule = new DeveloperModeModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleDeveloperMode->appendChild(developerModeModule);
    moduleInterface->appendChild(moduleDeveloperMode);

    //二级菜单--开发者模式
    ModuleObject *moduleUserExperienceProgram = new ModuleObject(tr("User Experience Program"), tr("User Experience Program"), this);
    moduleUserExperienceProgram->setChildType(ModuleObject::ChildType::Page);
    UserExperienceProgramModule *userExperienceProgramModule = new UserExperienceProgramModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleUserExperienceProgram->appendChild(userExperienceProgramModule);
    moduleInterface->appendChild(moduleUserExperienceProgram);

    return moduleInterface;
}

QWidget *DeveloperModeModule::page()
{
    DeveloperModeWidget *w = new DeveloperModeWidget;
    w->setModel(m_model);
    connect(w, &DeveloperModeWidget::requestLogin, m_worker, &CommonInfoWork::login);
    connect(w, &DeveloperModeWidget::enableDeveloperMode, this, [=](bool enabled) {
        m_worker->setEnableDeveloperMode(enabled);
    });
    return w;
}

QWidget *UserExperienceProgramModule::page()
{
    UserExperienceProgramWidget *w = new UserExperienceProgramWidget();
    w->setModel(m_model);
    if (DSysInfo::uosEditionType() != DSysInfo::UosCommunity) {
        bool isEnabled = m_worker->defaultUeProgram();
        w->setDefaultUeProgram(isEnabled);
    }

    connect(w, &UserExperienceProgramWidget::enableUeProgram, m_worker, &CommonInfoWork::setUeProgram);
    return w;
}

QWidget *BootModule::page()
{
    m_worker->loadGrubSettings();
    BootWidget *w = new BootWidget();
    w->setModel(m_model);
    connect(w, &BootWidget::bootdelay, m_worker, &CommonInfoWork::setBootDelay);
    connect(w, &BootWidget::enableTheme, m_worker, &CommonInfoWork::setEnableTheme);
    connect(w, &BootWidget::enableGrubEditAuth, m_worker, [this, w](bool value){
        if (value) {
            w->showGrubEditAuthPasswdDialog(false);
        } else {
            m_worker->disableGrubEditAuth();
        }
    });
    connect(w, &BootWidget::setGrubEditPasswd, m_worker, &CommonInfoWork::onSetGrubEditPasswd);
    connect(w, &BootWidget::defaultEntry, m_worker, &CommonInfoWork::setDefaultEntry);
    connect(w, &BootWidget::requestSetBackground, m_worker, &CommonInfoWork::setBackground);
    connect(m_worker, &CommonInfoWork::grubEditAuthCancel, w, &BootWidget::onGrubEditAuthCancel);
    connect(m_worker, &CommonInfoWork::showGrubEditAuthChanged, w, &BootWidget::setGrubEditAuthVisible);

    w->setGrubEditAuthVisible(m_model->isShowGrubEditAuth());
    return w;
}
