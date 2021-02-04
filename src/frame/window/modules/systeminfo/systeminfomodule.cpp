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

using namespace dcc::systeminfo;
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_work(nullptr)
    , m_model(nullptr)
    , m_sysinfoWidget(nullptr)
{
    m_frameProxy = frame;
}

SystemInfoModule::~SystemInfoModule()
{
}

void SystemInfoModule::initialize()
{
    if (m_model) {
        delete m_model;
    }
    m_model = new SystemInfoModel(this);
    m_work = new SystemInfoWork(m_model, this);

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

    QListView *list = m_sysinfoWidget->getSystemListViewPointer();
    SystemType type = Default;

    if (!list) {
        return 0;
    }

    if (path == "About This PC") {
        type = AboutThisPC;
    } else if (path == "Edition License") {
        type = EditionLicense;
    } else if (path == "End User License Agreement") {
        type = EndUserLicenseAgreement;
    } else if (path == "Privacy Policy") {
        type = PrivacyPolicy;
    }

    QModelIndex index = list->model()->index(type, 0);
    if (type > Default && type < MaxType) {
        index = list->model()->index(type, 0);
        list->setCurrentIndex(index);
        list->clicked(index);
    }

    return 0;
}

QStringList SystemInfoModule::availPage() const
{
    QStringList availList;
    availList << "About This PC" << "Edition License" << "End User License Agreement" << "Privacy Policy";

    return availList;
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

