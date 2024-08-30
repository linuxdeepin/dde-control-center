// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "commoninfomodule.h"

#include "bootwidget.h"
#include "dcclistview.h"
#include "developermodewidget.h"
#include "interface/pagemodule.h"
#include "moduleobject.h"
#include "plymouthdisplay.h"
#include "src/frame/utils.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "src/plugin-commoninfo/operation/commoninfowork.h"
#include "userexperienceprogramwidget.h"
#include "widgets/itemmodule.h"

#include <qstandarditemmodel.h>

#include <DIconTheme>
#include <DSysInfo>
#include <DTipLabel>

#include <QApplication>

using Dtk::Widget::DStandardItem;

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
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
    // 一级菜单--通用设置
    CommonInfoModule *moduleInterface = new CommonInfoModule();
    moduleInterface->setName("commoninfo");
    moduleInterface->setDisplayName(tr("General Settings"));
    moduleInterface->setIcon(DIconTheme::findQIcon("dcc_nav_commoninfo"));

    // 二级菜单--启动菜单
    ModuleObject *moduleBootMenu = new PageModule("bootMenu", tr("Boot Menu"));
    BootModule *bootModule =
            new BootModule(moduleInterface->model(), moduleInterface->worker(), moduleBootMenu);
    moduleBootMenu->appendChild(bootModule);
    moduleInterface->appendChild(moduleBootMenu);

    moduleInterface->appendChild(
            new PlyMouthModule(moduleInterface->model(), moduleInterface->worker()));

    // 服务器版/社区版
    if (!IS_SERVER_SYSTEM && !IS_COMMUNITY_SYSTEM && DSysInfo::isDeepin()) {
        if (DSysInfo::uosEditionType() != DSysInfo::UosEuler
            || DSysInfo::uosEditionType() != DSysInfo::UosEnterpriseC) {
            // 二级菜单--开发者模式
            ModuleObject *moduleDeveloperMode =
                    new PageModule("developerMode", tr("Developer Mode"));
            DeveloperModeModule *developerModeModule =
                    new DeveloperModeModule(moduleInterface->model(),
                                            moduleInterface->worker(),
                                            moduleBootMenu);
            moduleDeveloperMode->appendChild(developerModeModule);
            moduleInterface->appendChild(moduleDeveloperMode);
        }

        // 二级菜单--用户体验计划
        ModuleObject *moduleUserExperienceProgram =
                new PageModule("userExperienceProgram", tr("User Experience Program"));
        UserExperienceProgramModule *userExperienceProgramModule =
                new UserExperienceProgramModule(moduleInterface->model(),
                                                moduleInterface->worker(),
                                                moduleBootMenu);
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
    connect(w,
            &UserExperienceProgramWidget::enableUeProgram,
            m_worker,
            &CommonInfoWork::setUeProgram);
    connect(w, &UserExperienceProgramWidget::destroyed, m_worker, &CommonInfoWork::closeUeProgram);
    return w;
}

QWidget *BootModule::page()
{
    BootWidget *w = new BootWidget();
    w->setModel(m_model);
    connect(w, &BootWidget::bootdelay, m_worker, &CommonInfoWork::setBootDelay);
    connect(w, &BootWidget::enableTheme, m_worker, &CommonInfoWork::setEnableTheme);
    connect(w, &BootWidget::enableGrubEditAuth, m_worker, [this, w](bool value) {
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

PlyMouthModule::PlyMouthModule(CommonInfoModel *model, CommonInfoWork *work, QObject *parent)
    : PageModule("plymouthAnimation", tr("Boot Animation"), parent)
    , m_model(model)
    , m_work(work)
{
    appendChild(new ItemModule("", "", this, &PlyMouthModule::initPlyMouthDisplay, false));
    appendChild(new ItemModule("plymouthScale",
                               "",
                               this,
                               &PlyMouthModule::initPlymouthScale,
                               false));
}

QWidget *PlyMouthModule::initPlyMouthDisplay(ModuleObject *module)
{
    auto displayItem = new PlyMouthDisplayItem;

    displayItem->setLogoPixmap(m_work->getPlymouthFilePixmap());
    connect(m_model, &CommonInfoModel::plymouthThemeChanged, displayItem, [displayItem, this] {
        displayItem->setLogoPixmap(m_work->getPlymouthFilePixmap());
    });

    return displayItem;
}

QWidget *PlyMouthModule::initPlymouthScale(ModuleObject *module)
{
    Q_UNUSED(module)
    DCCListView *plymouthView = new DCCListView;
    QStandardItemModel *plymouthModel = new QStandardItemModel;

    DStandardItem *smallOne = new DStandardItem;
    smallOne->setData(tr("Small Size"), Qt::DisplayRole);
    smallOne->setData(1, Dtk::UserRole);

    DStandardItem *bingOne = new DStandardItem;
    bingOne->setData(tr("Big Size"), Qt::DisplayRole);
    bingOne->setData(2, Dtk::UserRole);

    plymouthModel->appendRow(smallOne);
    plymouthModel->appendRow(bingOne);

    plymouthView->setModel(plymouthModel);

    auto plymouthValueViewChanged = [plymouthView, plymouthModel](int scale) {
        if (scale == 0 || scale > 2) {
            return;
        }
        int row = scale - 1;
        plymouthView->setCurrentIndex(plymouthModel->index(row, 0));
        for (int i = 0; i < 2; ++i) {
            auto item = plymouthModel->item(i);
            item->setCheckState(row == i ? Qt::Checked : Qt::Unchecked);
        }
    };
    plymouthValueViewChanged(m_model->plymouthScale());

    auto handleDisableChanged = [plymouthModel](bool disable) {
        for (int i = 0; i < 2; ++i) {
            auto item = plymouthModel->item(i);
            item->setEnabled(!disable);
        }
    };

    handleDisableChanged(m_work->isSettingPlymouth());

    connect(m_model,
            &CommonInfoModel::plymouthScaleChanged,
            plymouthView,
            plymouthValueViewChanged);

    connect(m_work, &CommonInfoWork::settingScaling, plymouthView, handleDisableChanged);

    connect(plymouthView, &QListView::clicked, m_work, [this](const QModelIndex &index) {
        int scale = index.row() + 1;
        m_work->setPlymouthFactor(scale);
    });
    return plymouthView;
}
