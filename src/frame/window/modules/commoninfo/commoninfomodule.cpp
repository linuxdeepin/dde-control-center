/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/modules/commoninfo/commoninfowork.h"
#include "window/modules/commoninfo/commoninfowidget.h"
#include "window/modules/commoninfo/bootwidget.h"
#include "window/modules/commoninfo/userexperienceprogramwidget.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::commoninfo;

CommonInfoModule::CommonInfoModule(dccV20::FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_commonWidget(nullptr)
    , m_bootWidget(nullptr)
    , m_ueProgramWidget(nullptr)
{
}

CommonInfoModule::~CommonInfoModule()
{
    m_commonWork->deleteLater();
    m_commonModel->deleteLater();

    if (m_commonWidget) {
        m_commonWidget->deleteLater();
    }
}

void CommonInfoModule::initialize()
{
    m_commonModel = new CommonInfoModel();
    m_commonWork = new CommonInfoWork(m_commonModel);

    m_commonWork->moveToThread(qApp->thread());
    m_commonModel->moveToThread(qApp->thread());
}

const QString CommonInfoModule::name() const
{
    return QStringLiteral("commoninfo");
}

void CommonInfoModule::active()
{
//    mCommonWork->activate();
    m_commonWidget = new CommonInfoWidget();
    connect(m_commonWidget, &CommonInfoWidget::requestShowBootWidget, this, &CommonInfoModule::onShowBootWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowDeveloperModeWidget, this, &CommonInfoModule::onShowDeveloperWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowUEPlanWidget, this, &CommonInfoModule::onShowUEPlanWidget);
    connect(m_commonWidget, &CommonInfoWidget::requestShowTabletModeWidget, this, &CommonInfoModule::onShowTabletModeWidget);
    m_frameProxy->pushWidget(this, m_commonWidget);

    onShowBootWidget();
}

void CommonInfoModule::deactive()
{
    m_commonWork->deactivate();
}

void CommonInfoModule::load(QString path)
{
    if (!m_commonWidget) {
        active();
    }

    QListView *list = m_commonWidget->getCommonListView();
    if (!list) {
        return;
    }

    int indexRow = 0;
    if (path == "Boot Menu") {
        indexRow = 0;
    } else if (path == "Developer Mode") {
        // 为开发者模式的search预留
        //indexRow = 1;
    } else if (path == "User Experience Program") {
        // 为用户体验计划的search预留
        indexRow = 2;
    } else if (path == "Tablet Plan") {
        // 为平板模式的search预留
        //indexRow = 3;
    }

    QModelIndex idx = list->model()->index(indexRow, 0);
    list->setCurrentIndex(idx);
    list->clicked(idx);
}

void CommonInfoModule::onShowBootWidget()
{
    m_commonWork->loadGrubSettings();
    initBootWidget();
    m_frameProxy->pushWidget(this, m_bootWidget);
}

void CommonInfoModule::onShowDeveloperWidget()
{

}

void CommonInfoModule::onShowUEPlanWidget()
{
    initUeProgramWidget();
    m_frameProxy->pushWidget(this, m_ueProgramWidget);
}

// 以下内容为平板模式做预留
void CommonInfoModule::onShowTabletModeWidget()
{

}

void CommonInfoModule::initBootWidget()
{
    m_bootWidget = new BootWidget;
    m_bootWidget->setModel(m_commonModel);

    connect(m_bootWidget, &BootWidget::bootdelay, m_commonWork, &CommonInfoWork::setBootDelay);
    connect(m_bootWidget, &BootWidget::enableTheme, m_commonWork, &CommonInfoWork::setEnableTheme);
    connect(m_bootWidget, &BootWidget::defaultEntry, m_commonWork, &CommonInfoWork::setDefaultEntry);
    connect(m_bootWidget, &BootWidget::requestSetBackground, m_commonWork, &CommonInfoWork::setBackground);
}

void CommonInfoModule::initUeProgramWidget()
{
    m_ueProgramWidget = new UserExperienceProgramWidget();
    m_ueProgramWidget->setModel(m_commonModel);
    m_ueProgramWidget->setDefaultUeProgram(m_commonWork->defaultUeProgram());
    connect(m_ueProgramWidget, &UserExperienceProgramWidget::enableUeProgram,
            m_commonWork, &CommonInfoWork::setUeProgram);
}
