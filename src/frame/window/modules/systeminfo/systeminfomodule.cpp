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

#include "systeminfomodule.h"
#include "systeminfowidget.h"
#include "nativeinfowidget.h"
#include "versionprotocolwidget.h"
#include "userlicensewidget.h"
#include "privacypolicywidget.h"

#include "modules/systeminfo/systeminfowork.h"
#include "modules/systeminfo/systeminfomodel.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

using namespace dcc::systeminfo;
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_work(nullptr)
    , m_model(nullptr)
    , m_sysinfoWidget(nullptr)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    m_frameProxy = frame;
    GSettingWatcher::instance()->insertState("aboutThisPc");
    GSettingWatcher::instance()->insertState("editionLicense");
    GSettingWatcher::instance()->insertState("endUserLicenseAgreement");
    GSettingWatcher::instance()->insertState("privacyPolicy");
}

SystemInfoModule::~SystemInfoModule()
{
}

void SystemInfoModule::preInitialize(bool sync, dccV20::FrameProxyInterface::PushType)
{
    Q_UNUSED(sync)
    if (m_model) {
        delete m_model;
    }
    m_model = new SystemInfoModel(this);
    m_work = new SystemInfoWork(m_model, this);

    addChildPageTrans();
    initSearchData();
}

void SystemInfoModule::initialize()
{
    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

    m_work->activate();
}

void SystemInfoModule::reset()
{
}

void SystemInfoModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void SystemInfoModule::active()
{
    m_sysinfoWidget = new SystemInfoWidget;
    m_sysinfoWidget->setVisible(false);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowAboutNative, this, &SystemInfoModule::onShowAboutNativePage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowVersionProtocol, this, &SystemInfoModule::onVersionProtocolPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowEndUserLicenseAgreement, this, &SystemInfoModule::onShowEndUserLicenseAgreementPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowPrivacyPolicy, this, &SystemInfoModule::onShowPrivacyPolicyPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestUpdateSecondMenu, this, [=] (bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop)
            m_frameProxy->popWidget(this);
        m_sysinfoWidget->showDefaultWidget();
    });

    m_frameProxy->pushWidget(this, m_sysinfoWidget);
    m_sysinfoWidget->setVisible(true);
    m_sysinfoWidget->setCurrentIndex(0);
}

const QString SystemInfoModule::name() const
{
    return QStringLiteral("systeminfo");
}

const QString SystemInfoModule::displayName() const
{
    return tr("System Info");
}

int SystemInfoModule::load(const QString &path)
{
    if (!m_sysinfoWidget) {
        active();
    }

    return m_sysinfoWidget->showPath(path);
}

QStringList SystemInfoModule::availPage() const
{
    QStringList availList;
    availList << "About This PC" << "Edition License" << "End User License Agreement" << "Privacy Policy";

    return availList;
}

void SystemInfoModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //systeminfo
        m_frameProxy->addChildPageTrans("About This PC", tr("About This PC"));
        m_frameProxy->addChildPageTrans("Edition License", tr("Edition License"));
        m_frameProxy->addChildPageTrans("End User License Agreement", tr("End User License Agreement"));
        m_frameProxy->addChildPageTrans("Privacy Policy", tr("Privacy Policy"));
    }
}

void SystemInfoModule::initSearchData()
{
    QString module = tr("System Info");
    QString aboutThisPC = tr("About This PC");
    QString edtLicense = tr("Edition License");
    QString endUserLicenseAgree = tr("End User License Agreement");
    QString privacyPolicy = tr("Privacy Policy");
    QStringList gsSecondList;
    gsSecondList << "aboutThisPc" << "editionLicense" << "endUserLicenseAgreement" << "privacyPolicy";
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

    auto func_aboutthispc_changed = [ = ] {
        bool bAountThiePc = func_is_visible("aboutThisPc");
        m_frameProxy->setWidgetVisible(module, aboutThisPC, bAountThiePc);
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Computer Name:"), bAountThiePc);
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("OS Name:"), bAountThiePc);
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Version:"), bAountThiePc);
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Edition:"), bAountThiePc && func_is_visible("edition", true));
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Type:"), bAountThiePc);
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Authorization"), bAountThiePc && func_is_visible("systeminfoNativeinfoAuthorized", true));
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Kernel:"), bAountThiePc &&  func_is_visible("systeminfoNativeinfoKernel", true));
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Processor:"), bAountThiePc && func_is_visible("systeminfoNativeinfoProcessor", true));
        m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Memory:"), bAountThiePc && func_is_visible("systeminfoNativeinfoMemory", true));
    };

    auto func_edtlicense_changed = [ = ] {
        bool bEdtLicense = func_is_visible("editionLicense");
        m_frameProxy->setWidgetVisible(module, edtLicense, bEdtLicense);
        m_frameProxy->setDetailVisible(module, edtLicense, edtLicense, bEdtLicense);
    };

    auto func_enduserlicenseagreen_changed = [ = ] {
        bool bEndUserLicenseAgree = func_is_visible("endUserLicenseAgreement");
        m_frameProxy->setWidgetVisible(module, endUserLicenseAgree, bEndUserLicenseAgree);
        m_frameProxy->setDetailVisible(module, endUserLicenseAgree, endUserLicenseAgree, bEndUserLicenseAgree);
    };

    auto func_privacypolicy_changed = [ = ] {
        bool bPrivacyPolicy = func_is_visible("privacyPolicy");
        m_frameProxy->setWidgetVisible(module, privacyPolicy, bPrivacyPolicy);
        m_frameProxy->setDetailVisible(module, privacyPolicy, privacyPolicy, bPrivacyPolicy);
    };


    auto func_process_all = [ = ]() {

        m_frameProxy->setModuleVisible(module, true);

        func_aboutthispc_changed();

        func_edtlicense_changed();

        func_enduserlicenseagreen_changed();

        func_privacypolicy_changed();
     };

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
            return;
        }

        if (gsSecondList.contains(gsetting)) {
            if (gsettingsMap.value(gsetting) == GSettingWatcher::instance()->get(gsetting).toBool()) {
                return;
            }

            if ("aboutThisPc" == gsetting) {
                func_aboutthispc_changed();
            } else if ("endUserLicenseAgreement" == gsetting) {
                func_enduserlicenseagreen_changed();
            } else if ("privacyPolicy" == gsetting) {
                func_privacypolicy_changed();
            } else if ("editionLicense" == gsetting) {
                func_edtlicense_changed();
            } else {
                qDebug() << " not contains the gsettings : " << gsetting << state;
                return;
            }
        } else {
            if (gsettingsMap.value(gsetting) == (GSettingWatcher::instance()->get(gsetting).toString() != "Hidden")) {
                return;
            }

            bool isAbout = func_is_visible("aboutThisPc");
            if ("edition" == gsetting) {
                m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Edition:"), isAbout && func_is_visible("edition", true));
            } else if ("systeminfoNativeinfoAuthorized" == gsetting) {
                m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Authorization"), isAbout && func_is_visible("systeminfoNativeinfoAuthorized", true));
            } else if ("systeminfoNativeinfoKernel" == gsetting) {
                m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Kernel:"), isAbout &&  func_is_visible("systeminfoNativeinfoKernel", true));
            } else if ("systeminfoNativeinfoProcessor" == gsetting) {
                m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Processor:"), isAbout && func_is_visible("systeminfoNativeinfoProcessor", true));
            } else if ("systeminfoNativeinfoMemory" == gsetting) {
                m_frameProxy->setDetailVisible(module, aboutThisPC, tr("Memory:"), isAbout && func_is_visible("systeminfoNativeinfoMemory", true));
            } else {
                qDebug() << " not contains the gsettings : " << gsetting << state;
                return;
            }
        }

        qWarning() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void SystemInfoModule::onShowAboutNativePage()
{
    NativeInfoWidget *w = new NativeInfoWidget(m_model);
    w->setVisible(false);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
    //showActivatorDialog
    connect(w, &NativeInfoWidget::clickedActivator, m_work, &SystemInfoWork::showActivatorDialog);
}

void SystemInfoModule::onVersionProtocolPage()
{
    VersionProtocolWidget *w = new VersionProtocolWidget;
    w->setVisible(false);
    connect(w, &VersionProtocolWidget::loadTextFinished, [ = ](){
        m_frameProxy->pushWidget(this, w);
        w->setVisible(true);
    });
}

void SystemInfoModule::onShowEndUserLicenseAgreementPage()
{
    UserLicenseWidget *w = new UserLicenseWidget;
    w->setVisible(false);
    connect(w, &UserLicenseWidget::loadTextFinished, [ = ](){
        m_frameProxy->pushWidget(this, w);
        w->setVisible(true);
    });
}

void SystemInfoModule::onShowPrivacyPolicyPage()
{
     PrivacyPolicyWidget *w = new PrivacyPolicyWidget;
     w->setVisible(false);
     m_frameProxy->pushWidget(this, w);
     w->setVisible(true);
}


