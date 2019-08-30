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
{

}

DefaultAppsModule::~DefaultAppsModule()
{
    m_defAppModel->deleteLater();
    m_defAppWorker->deleteLater();
}

void DefaultAppsModule::initialize()
{
    m_defAppModel  = new dcc::defapp::DefAppModel;
    m_defAppWorker = new dcc::defapp::DefAppWorker(m_defAppModel);
    m_defAppModel->moveToThread(qApp->thread());
    m_defAppWorker->moveToThread(qApp->thread());
    m_defAppWorker->onGetListApps();
}

void DefaultAppsModule::active()
{
    DefaultAppsWidget* defaultappsWidget = new DefaultAppsWidget;

    connect(this, &DefaultAppsModule::requestSetDefappCategory, defaultappsWidget, &DefaultAppsWidget::setCurrentIndex);
    connect(defaultappsWidget, &DefaultAppsWidget::requestCategoryClicked, this, &DefaultAppsModule::showDetailWidget);
    m_frameProxy->pushWidget(this, defaultappsWidget);
    //显示默认页
    showDetailWidget(dcc::defapp::DefAppWorker::Browser);
}

const QString DefaultAppsModule::name() const
{
    return QStringLiteral("defapp");
}

void DefaultAppsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void DefaultAppsModule::load(QString path)
{
    dcc::defapp::DefAppWorker::DefaultAppsCategory currentCategory = dcc::defapp::DefAppWorker::Browser;
    QString loadPath = path.split("/").at(0);

    if (loadPath == QStringLiteral("Webpage")) {
        return;
    } else if (loadPath == QStringLiteral("Mail")) {
        currentCategory = dcc::defapp::DefAppWorker::Mail;
    } else if (loadPath == QStringLiteral("Text")) {
        currentCategory = dcc::defapp::DefAppWorker::Text;
    } else if (loadPath == QStringLiteral("Music")) {
        currentCategory = dcc::defapp::DefAppWorker::Music;
    } else if (loadPath == QStringLiteral("Video")) {
        currentCategory = dcc::defapp::DefAppWorker::Video;
    } else if (loadPath == QStringLiteral("Picture")) {
        currentCategory = dcc::defapp::DefAppWorker::Picture;
    } else if (loadPath == QStringLiteral("Terminal")) {
        currentCategory = dcc::defapp::DefAppWorker::Terminal;
    }

    showDetailWidget(currentCategory);
    Q_EMIT requestSetDefappCategory(currentCategory);
}

void DefaultAppsModule::showDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category) {
    DefappDetailWidget* detailWidget = new DefappDetailWidget(category);

    detailWidget->setModel(m_defAppModel);
    connect(detailWidget, &DefappDetailWidget::requestSetDefaultApp, m_defAppWorker, &dcc::defapp::DefAppWorker::onSetDefaultApp); //设置默认程序
    connect(detailWidget, &DefappDetailWidget::requestDelUserApp, m_defAppWorker, &dcc::defapp::DefAppWorker::onDelUserApp);
    connect(detailWidget, &DefappDetailWidget::requestCreateFile, m_defAppWorker, &dcc::defapp::DefAppWorker::onCreateFile);
    m_frameProxy->pushWidget(this, detailWidget);
}
