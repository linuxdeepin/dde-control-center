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

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::commoninfo;

CommonInfoModule::CommonInfoModule(dccV20::FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_commonWidget(nullptr)
    , m_bootWidget(nullptr)
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
    // 以下内容为平板模式做预留
    //connect(mCommonWidget, SIGNAL(requestShowTabletModeWidget()), this, SLOT(onShowTabletModeWidget()));
    m_frameProxy->pushWidget(this, m_commonWidget);
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

    if (path == "Boot Menu") {
        QModelIndex idx = list->model()->index(0, 0);
        list->setCurrentIndex(idx);
        list->clicked(idx);
    } else if (path == "Developer Mode") {
        // 为开发者设计的search预留
    } else if (path == "User Experience Program") {
        // 为用户体验计划的search预留
    }
}

void CommonInfoModule::onShowBootWidget()
{
    m_commonWork->loadGrubSettings();
    initBootWidget();
    if (!m_bootWidget) {
        return;
    }
    m_frameProxy->pushWidget(this, m_bootWidget);
}

void CommonInfoModule::onShowDeveloperWidget()
{

}

void CommonInfoModule::onShowUEPlanWidget()
{

}

// 以下内容为平板模式做预留
//void CommonInfoModule::onShowTabletModeWidget()
//{

//}

void CommonInfoModule::initBootWidget()
{
    m_bootWidget = new BootWidget;
    m_bootWidget->setModel(m_commonModel);

    connect(m_bootWidget, &BootWidget::bootdelay, m_commonWork, &CommonInfoWork::setBootDelay);
    connect(m_bootWidget, &BootWidget::enableTheme, m_commonWork, &CommonInfoWork::setEnableTheme);
    connect(m_bootWidget, &BootWidget::defaultEntry, m_commonWork, &CommonInfoWork::setDefaultEntry);
    connect(m_bootWidget, &BootWidget::requestSetBackground, m_commonWork, &CommonInfoWork::setBackground);
}
