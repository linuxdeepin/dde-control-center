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
#include "modules/systeminfo/systeminfowork.h"
#include "modules/systeminfo/systeminfomodel.h"

using namespace dcc::systeminfo;
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
{
    m_frameProxy = frame;
}

SystemInfoModule::~SystemInfoModule()
{
}

void SystemInfoModule::initialize()
{
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
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowAboutNative, this, &SystemInfoModule::onShowAboutNativePage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowVersionProtocol, this, &SystemInfoModule::onVersionProtocolPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowEndUserLicenseAgreement, this, &SystemInfoModule::onShowEndUserLicenseAgreementPage);
    m_frameProxy->pushWidget(this, m_sysinfoWidget);
    m_sysinfoWidget->setCurrentIndex(0);
}

const QString SystemInfoModule::name() const
{
    return QStringLiteral("systeminfo");
}

void SystemInfoModule::load(QString path)
{
    if (!m_sysinfoWidget) {
        active();
    }

    if (path == "About This PC") {
        m_sysinfoWidget->setCurrentIndex(0);
    } else if (path == "Edition License") {
        m_sysinfoWidget->setCurrentIndex(1);
    } else if (path == "End User License Agreement") {
        m_sysinfoWidget->setCurrentIndex(2);
    }
}

void SystemInfoModule::onShowAboutNativePage()
{
    NativeInfoWidget *w = new NativeInfoWidget(m_model);
    m_frameProxy->pushWidget(this, w);
}

void SystemInfoModule::onVersionProtocolPage()
{
    VersionProtocolWidget *w = new VersionProtocolWidget;
    m_frameProxy->pushWidget(this, w);
}

void SystemInfoModule::onShowEndUserLicenseAgreementPage()
{
    UserLicenseWidget *w = new UserLicenseWidget;
    m_frameProxy->pushWidget(this, w);
}
