// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "commoninfomodule.h"
#include "widgets/multiselectlistview.h"
#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/modules/commoninfo/commoninfowork.h"
#include "window/modules/commoninfo/commoninfowidget.h"
#include "window/modules/commoninfo/bootwidget.h"
#include "window/modules/commoninfo/userexperienceprogramwidget.h"
#include "window/modules/commoninfo/developermodewidget.h"
#include "window/utils.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"
#include "window/insertplugin.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::commoninfo;

CommonInfoModule::CommonInfoModule(dccV20::FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_commonWidget(nullptr)
    , m_bootWidget(nullptr)
    , m_ueProgramWidget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
#ifdef DCC_DISABLE_GRUB
    //如果服务器版本，且不显示GRUB(arm),则整个模块不显示
    if (IsServerSystem)
        m_frameProxy->setModuleVisible(this, false);
#endif
    GSettingWatcher::instance()->insertState("bootMenu");
    GSettingWatcher::instance()->insertState("developerMode");
    GSettingWatcher::instance()->insertState("userExperienceProgram");
}

CommonInfoModule::~CommonInfoModule()
{
    if (m_commonWork) {
//        m_commonWork->deleteLater();
        delete m_commonWork;
        m_commonWork = nullptr;
    }

    if (m_commonModel)
        m_commonModel->deleteLater();
    m_commonModel = nullptr;
}

void CommonInfoModule::preInitialize(bool sync , FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync);
    Q_UNUSED(pushtype);
    addChildPageTrans();
    setAvailable(DSysInfo::uosEditionType() != DSysInfo::UosEuler);
    initSearchData();
}

void CommonInfoModule::initialize()
{
    if (m_commonModel) {
        delete m_commonModel;
    }
    m_commonModel = new CommonInfoModel();
    m_commonWork = new CommonInfoWork(m_commonModel);

    m_commonWork->moveToThread(qApp->thread());
    m_commonModel->moveToThread(qApp->thread());
}

const QString CommonInfoModule::name() const
{
    return QStringLiteral("commoninfo");
}

const QString CommonInfoModule::displayName() const
{
    return tr("General Settings");
}

void CommonInfoModule::active()
{
    m_commonWidget = new CommonInfoWidget();
    m_commonWidget->setVisible(false);
    connect(m_commonWidget, &CommonInfoWidget::requestShowBootWidget, this, &CommonInfoModule::onShowBootWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowDeveloperModeWidget, this, &CommonInfoModule::onShowDeveloperWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowUEPlanWidget, this, &CommonInfoModule::onShowUEPlanWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowTabletModeWidget, this, &CommonInfoModule::onShowTabletModeWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestUpdateSecondMenu, this, [=] (bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop)
            m_frameProxy->popWidget(this);
        m_commonWidget->showDefaultWidget();
    });
    m_frameProxy->pushWidget(this, m_commonWidget);
    m_commonWidget->setVisible(true);
    m_commonWidget->showDefaultWidget();
}

void CommonInfoModule::deactive()
{
    m_commonWork->deactivate();
}

int CommonInfoModule::load(const QString &path)
{
    if (!m_commonWidget) {
        active();
    }
    const QStringList &page = availPage();
    int i = page.indexOf(path);
    if (i != -1) {
        m_commonWidget->setCurrentIndex(i);
        return 0;
    }
    return -1;
}

QStringList CommonInfoModule::availPage() const
{
    QStringList sl;
    sl << "Boot Menu";

    if (!IsServerSystem && !IsCommunitySystem) {
        sl << "User Experience Program" << "Developer Mode";
    }
    sl.append(InsertPlugin::instance()->availPages(name()));
    return sl;
}

void CommonInfoModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //commoninfo
        m_frameProxy->addChildPageTrans("Boot Menu", tr("Boot Menu"));
        m_frameProxy->addChildPageTrans("Developer Mode", tr("Developer Mode"));
        m_frameProxy->addChildPageTrans("User Experience Program", tr("User Experience Program"));
    }
}

void CommonInfoModule::onShowBootWidget()
{
    m_commonWork->loadGrubSettings();
    initBootWidget();
    m_frameProxy->pushWidget(this, m_bootWidget);
    m_bootWidget->setVisible(true);
}

void CommonInfoModule::onShowDeveloperWidget()
{
    DeveloperModeWidget *pWidget = new DeveloperModeWidget(m_pMainWindow);
    pWidget->setVisible(false);
    pWidget->setModel(m_commonModel);
    connect(pWidget, &DeveloperModeWidget::requestLogin, m_commonWork, &CommonInfoWork::login);
    connect(pWidget, &DeveloperModeWidget::enableDeveloperMode, this, [=](bool enabled) {
        m_commonWork->setEnableDeveloperMode(enabled, m_pMainWindow);
    });
    m_frameProxy->pushWidget(this, pWidget);
    pWidget->setVisible(true);
}

void CommonInfoModule::onShowUEPlanWidget()
{
    initUeProgramWidget();
    m_frameProxy->pushWidget(this, m_ueProgramWidget);
    m_ueProgramWidget->setVisible(true);
}

// 以下内容为平板模式做预留
void CommonInfoModule::onShowTabletModeWidget()
{

}

void CommonInfoModule::initBootWidget()
{
    m_bootWidget = new BootWidget;
    m_bootWidget->setVisible(false);

#ifndef DCC_DISABLE_GRUB
    connect(m_bootWidget, &BootWidget::bootdelay, m_commonWork, &CommonInfoWork::setBootDelay);
    connect(m_bootWidget, &BootWidget::enableTheme, m_commonWork, &CommonInfoWork::setEnableTheme);
#endif//DCC_DISABLE_GRUB
    connect(m_bootWidget, &BootWidget::enableGrubEditAuth, m_commonWork, [this](bool value){
        if (value) {
            m_bootWidget->showGrubEditAuthPasswdDialog(false);
        } else {
            m_commonWork->disableGrubEditAuth();
        }
    });
    connect(m_bootWidget, &BootWidget::setGrubEditPasswd, m_commonWork, &CommonInfoWork::onSetGrubEditPasswd);
    connect(m_bootWidget, &BootWidget::defaultEntry, m_commonWork, &CommonInfoWork::setDefaultEntry);
#ifndef DCC_DISABLE_GRUB
    connect(m_bootWidget, &BootWidget::requestSetBackground, m_commonWork, &CommonInfoWork::setBackground);
#endif//DCC_DISABLE_GRUB
    connect(m_commonWork, &CommonInfoWork::grubEditAuthCancel, m_bootWidget, &BootWidget::onGrubEditAuthCancel);
    connect(m_commonWork, &CommonInfoWork::showGrubEditAuthChanged, m_bootWidget, &BootWidget::setGrubEditAuthVisible);

    m_bootWidget->setModel(m_commonModel);
    m_bootWidget->setGrubEditAuthVisible(m_commonModel->isShowGrubEditAuth());
}

void CommonInfoModule::initUeProgramWidget()
{
    m_ueProgramWidget = new UserExperienceProgramWidget();
    m_ueProgramWidget->setVisible(false);
    m_ueProgramWidget->setModel(m_commonModel);
    if (!IsCommunitySystem) {
        m_ueProgramWidget->setDefaultUeProgram(m_commonWork->isUeProgramEnabled());
    }
    connect(m_ueProgramWidget, &UserExperienceProgramWidget::enableUeProgram, this, [=](bool enabled) {
        MainWindow *pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
        m_commonWork->setUeProgram(enabled, pMainWindow);
    });
}

void CommonInfoModule::initSearchData()
{
    const QString& module = displayName();
    const QString& bootMenu = tr("Boot Menu");
    const QString& startupDelay = tr("Startup Delay");
    const QString& theme = tr("Theme");
    const QString& auth = tr("GRUB Authentication");
    const QString& developerMode = tr("Developer Mode");
    const QString& experienceProgram = tr("Join User Experience Program");
    const QString& userExperienceProgram = tr("User Experience Program");

    const QStringList& gsSecondList {
        "bootMenu"
        , "developerMode"
        , "userExperienceProgram"
    };

    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings, bool state = false) {
        if ("" == gsettings) {
            return false;
        }

        bool ret = false;
        if (!state) {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }
        gsettingsMap.insert(gsettings, ret);
        return ret;
    };


    auto func_bootMenu_changed = [ = ] {
        bool visible = false;
        visible = func_is_visible("bootMenu");
        m_frameProxy->setWidgetVisible(module, bootMenu, visible);
#ifndef DCC_DISABLE_GRUB
        m_frameProxy->setDetailVisible(module, bootMenu, startupDelay, visible && func_is_visible("commoninfoBootBootdelay", true));
#ifndef DCC_DISABLE_GRUB_THEME
        m_frameProxy->setDetailVisible(module, bootMenu, theme, visible && func_is_visible("commoninfoBootTheme", true));
#endif
#endif
        m_frameProxy->setDetailVisible(module, bootMenu, auth, visible && (m_commonModel ? m_commonModel->isShowGrubEditAuth() : true));
    };

    auto func_developerMode_changed = [ = ] {
        bool visible = func_is_visible("developerMode");
        m_frameProxy->setWidgetVisible(module, developerMode, visible && !IsServerSystem && !IsCommunitySystem
            && !(DSysInfo::uosEditionType() == DSysInfo::UosEuler || DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC));
        m_frameProxy->setDetailVisible(module, developerMode, tr("Request Root Access"), visible);
    };

    auto func_userExperienceProgram_changed = [ = ] {
        bool visible = func_is_visible("userExperienceProgram");
        m_frameProxy->setWidgetVisible(module, userExperienceProgram, visible && !IsServerSystem && !IsCommunitySystem);
        m_frameProxy->setDetailVisible(module, userExperienceProgram, experienceProgram, visible && !IsServerSystem && !IsCommunitySystem);
    };


    auto func_process_all = [=]() {
        func_bootMenu_changed();

        func_developerMode_changed();

        func_userExperienceProgram_changed();
    };



    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
               return;
        }

        if (gsSecondList.contains(gsetting)) {
           if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool()) {
               return;
           }

           if ("bootMenu" == gsetting) {
               func_bootMenu_changed();
           } else if ("developerMode" == gsetting) {
               func_developerMode_changed();
           } else if ("userExperienceProgram" == gsetting) {
               func_userExperienceProgram_changed();
           } else {
               qDebug() << " not contains the gsettings : " << gsetting << state;
               return;
           }
        } else {
           if (gsettingsMap.value(gsetting) == (GSettingWatcher::instance()->get(gsetting).toString() != "Hidden")) {
               return;
           }

           bool bootMenuVisible = func_is_visible("bootMenu");
           if ("commoninfoBootBootdelay" == gsetting) {
               m_frameProxy->setDetailVisible(module, bootMenu, startupDelay, bootMenuVisible && func_is_visible("commoninfoBootBootdelay", true));
           } else if ("commoninfoBootTheme" == gsetting) {
               m_frameProxy->setDetailVisible(module, bootMenu, theme, bootMenuVisible && func_is_visible("commoninfoBootTheme", true));
           } else {
               qWarning() << " not contains the gsettings : " << gsetting << state;
               return;
           }
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
     });

    connect(m_commonWork, &CommonInfoWork::showGrubEditAuthChanged, this, [=] (bool show) {
        m_frameProxy->setDetailVisible(module, bootMenu, auth, func_is_visible("bootMenu") && show);
    });

    func_process_all();
}
