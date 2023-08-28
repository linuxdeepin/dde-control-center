//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "commoninfomodule.h"
#include "interface/pagemodule.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "src/plugin-commoninfo/operation/commoninfowork.h"
#include "src/frame/utils.h"

#include "bootwidget.h"
#include "userexperienceprogramwidget.h"
#include "developermodewidget.h"

#include <DSysInfo>
#include <QApplication>

using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE

CommonInfoModule::CommonInfoModule(QObject *parent)
    : HListModule(parent)
    , m_worker(nullptr)
    , m_model(nullptr)
{
    m_model = new CommonInfoModel(this);
    m_worker = new CommonInfoWork(m_model, this);
}

CommonInfoModule::~CommonInfoModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void CommonInfoModule::active()
{
    m_worker->active();
}

QString CommonInfoPlugin::name() const
{
    return QStringLiteral("commoninfo");
}

ModuleObject *CommonInfoPlugin::module()
{   
    //一级菜单--通用设置
    CommonInfoModule *moduleInterface = new CommonInfoModule();
    moduleInterface->setName("commoninfo");
    moduleInterface->setDisplayName(tr("General Settings"));
    moduleInterface->setIcon(QIcon::fromTheme("dcc_nav_commoninfo"));

    //二级菜单--启动菜单
    ModuleObject *moduleBootMenu = new PageModule("bootMenu", tr("Boot Menu"));
    BootModule *bootModule = new BootModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleBootMenu->appendChild(bootModule);
    moduleInterface->appendChild(moduleBootMenu);

    // 服务器版/社区版
    if (!IS_SERVER_SYSTEM && !IS_COMMUNITY_SYSTEM && DSysInfo::isDeepin()) {
        if (DSysInfo::uosEditionType() != DSysInfo::UosEuler || DSysInfo::uosEditionType() != DSysInfo::UosEnterpriseC) {
            //二级菜单--开发者模式
            ModuleObject *moduleDeveloperMode = new PageModule("developerMode", tr("Developer Mode"));
            DeveloperModeModule *developerModeModule = new DeveloperModeModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
            moduleDeveloperMode->appendChild(developerModeModule);
            moduleInterface->appendChild(moduleDeveloperMode);
        }

        //二级菜单--用户体验计划
        ModuleObject *moduleUserExperienceProgram = new PageModule("userExperienceProgram", tr("User Experience Program"));
        UserExperienceProgramModule *userExperienceProgramModule = new UserExperienceProgramModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
        moduleUserExperienceProgram->appendChild(userExperienceProgramModule);
        moduleInterface->appendChild(moduleUserExperienceProgram);
    }

    return moduleInterface;
}

QString CommonInfoPlugin::location() const
{
    return "22";
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
    connect(w, &UserExperienceProgramWidget::enableUeProgram, m_worker, &CommonInfoWork::setUeProgram);
    connect(w, &UserExperienceProgramWidget::destroyed, m_worker, &CommonInfoWork::closeUeProgram);
    return w;
}

QWidget *BootModule::page()
{
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

    w->setGrubEditAuthVisible(m_model->isShowGrubEditAuth());
    return w;
}
