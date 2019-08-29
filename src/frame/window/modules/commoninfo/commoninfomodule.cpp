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

//#include "window/modules/commoninfo/bootwidget.h"
#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/modules/commoninfo/commoninfowork.h"
#include "window/modules/commoninfo/commoninfowidget.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::commoninfo;

CommonInfoModule::CommonInfoModule(dccV20::FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_commonWidget(nullptr)
      //, mBootWidget(nullptr)
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
    connect(m_commonWidget, SIGNAL(requestShowBootWidget()), this, SLOT(onShowBootWidget()));
    connect(m_commonWidget, SIGNAL(requestShowDeveloperModeWidget()), this, SLOT(onShowDeveloperWidget()));
    connect(m_commonWidget, SIGNAL(requestShowUEPlanWidget()), this, SLOT(onShowUEPlanWidget()));
    //connect(mCommonWidget, SIGNAL(requestShowTabletModeWidget()), this, SLOT(onShowTabletModeWidget()));
    m_frameProxy->pushWidget(this, m_commonWidget);
}

void CommonInfoModule::deactive()
{
    m_commonWork->deactivate();
}

void CommonInfoModule::load(QString path)
{

}

void CommonInfoModule::onShowBootWidget()
{
//    mCommonWork->loadGrubSettings();
//    initBootWidget();
//    if (!mBootWidget) {
//        return;
//    }
//    m_frameProxy->pushWidget(this, mBootWidget);
}

void CommonInfoModule::onShowDeveloperWidget()
{

}

void CommonInfoModule::onShowUEPlanWidget()
{

}

//void CommonInfoModule::onShowTabletModeWidget()
//{

//}

void CommonInfoModule::initBootWidget()
{
//    mBootWidget = new BootWidget;
//    mBootWidget->setModel(mCommonModel);

//    connect(mBootWidget, SIGNAL(bootdelay(bool)), mCommonWork, SLOT(setBootDelay(bool)));
//    connect(mBootWidget, SIGNAL(enableTheme(bool)),mCommonWork, SLOT(setEnableTheme(bool)));
//    connect(mBootWidget, SIGNAL(defaultEntry(QString)), mCommonWork, SLOT(setDefaultEntry(QString)));
//    connect(mBootWidget, &BootWidget::requestSetBackground, mCommonWork, &CommonInfoWork::setBackground);
}
