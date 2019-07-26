/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include "defaultappsmodule.h"
#include "defappwidget.h"
#include "defappdetailwidget.h"
#include "modules/defapp/defappmodel.h"
#include "modules/defapp/defappworker.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::defapp;

DefaultAppsModule::DefaultAppsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_defaultappsWidget(nullptr)
{

}

DefaultAppsModule::~DefaultAppsModule()
{
}

void DefaultAppsModule::initialize()
{
    m_defAppModel  = new dcc::defapp::DefAppModel;
    m_defAppWorker = new dcc::defapp::DefAppWorker(m_defAppModel);
    m_defAppModel->moveToThread(qApp->thread());
    m_defAppWorker->moveToThread(qApp->thread());
    m_defAppWorker->onGetListApps();
}

void DefaultAppsModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

void DefaultAppsModule::reset()
{

}

QWidget *DefaultAppsModule::moduleWidget()
{
    if (!m_defaultappsWidget) {
        m_defaultappsWidget = new DefaultAppsWidget;
        connect(m_defaultappsWidget, &DefaultAppsWidget::requestCategoryClicked, this, &DefaultAppsModule::showDetailWidget );
    }

    return m_defaultappsWidget;
}

const QString DefaultAppsModule::name() const
{
    return QStringLiteral("defapp");
}

void DefaultAppsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void DefaultAppsModule::showDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category) {
    DefappDetailWidget* detailWidget = new DefappDetailWidget(category);
    detailWidget->setModel(m_defAppModel);
    connect(detailWidget, &DefappDetailWidget::requestSetDefaultApp,   m_defAppWorker, &dcc::defapp::DefAppWorker::onSetDefaultApp); //设置默认程序
    connect(detailWidget, &DefappDetailWidget::requestDelUserApp,      m_defAppWorker, &dcc::defapp::DefAppWorker::onDelUserApp);
    connect(detailWidget, &DefappDetailWidget::requestCreateFile, m_defAppWorker, &dcc::defapp::DefAppWorker::onCreateFile);
    m_frameProxy->pushWidget(this,  detailWidget);
}
