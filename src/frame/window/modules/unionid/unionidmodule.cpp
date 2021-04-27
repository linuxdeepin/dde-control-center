/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
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

#include "interfaces/frameproxyinterface.h"
#include "unionidmodule.h"
#include "../../../modules/unionid/unionidworker.h"
#include "../../../modules/unionid/unionidmodel.h"
#include "window/utils.h"
#include "window/mainwindow.h"
#include "window/modules/unionid/notificationmanager.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;

UnionidModule::UnionidModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_worker(nullptr)
{
}

void UnionidModule::initialize()
{
}

const QString UnionidModule::name() const
{
    return QStringLiteral(/*"unionid"*/"cloudsync");
}

const QString UnionidModule::displayName() const
{
    return tr("Union ID");
}

void UnionidModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void UnionidModule::active()
{
    m_unionidWidget = new UnionidWidget;
    m_unionidWidget->setVisible(false);
    connect(m_unionidWidget, &UnionidWidget::requestSignInUser, m_worker, &dcc::unionid::UnionidWorker::signInUser, Qt::UniqueConnection);
    connect(m_unionidWidget, &UnionidWidget::requestLoginUser, m_worker, &dcc::unionid::UnionidWorker::loginUser, Qt::UniqueConnection);
    connect(m_unionidWidget, &UnionidWidget::requestSetAutoSync, m_worker, &dcc::unionid::UnionidWorker::setAutoSync, Qt::UniqueConnection);
    connect(m_unionidWidget, &UnionidWidget::requestLogoutUser, m_worker, &dcc::unionid::UnionidWorker::logoutUser, Qt::QueuedConnection);
    connect(m_unionidWidget, &UnionidWidget::requesUserDialog, m_worker, &dcc::unionid::UnionidWorker::requestModifyDialog, Qt::UniqueConnection);
    connect(m_unionidWidget, &UnionidWidget::requestSetModuleState, m_worker, &dcc::unionid::UnionidWorker::setSync, Qt::UniqueConnection);
//    connect(widget, &UnionidWidget::requestPopupDialog, this, &UnionidModule::onShowPopup, Qt::UniqueConnection);
    MainWindow *pMainWindow = static_cast<MainWindow *>(m_frameProxy);
    m_unionidWidget->setModel(m_model,pMainWindow);

    m_frameProxy->pushWidget(this, m_unionidWidget);
    m_unionidWidget->setVisible(true);

    m_worker->activate(); //refresh data
}

void UnionidModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync);
    Q_UNUSED(pushtype);
    m_model = new dcc::unionid::UnionidModel;
    m_worker = new dcc::unionid::UnionidWorker(m_model);

    qInfo() << " IsServerSystem" << IsServerSystem << m_model->syncIsValid();
    m_frameProxy->setModuleVisible(this, /*m_model->syncIsValid() && !IsServerSystem && false*/true);
}

QStringList UnionidModule::availPage() const
{
    QStringList sl;
    sl << "unionid " << "Unionid Sign In";
    return sl;
}

void UnionidModule::getAccessToken(const QString &code, const QString &state,const bool &bIsUIDWidget)
{ 
    //当前界面是unoinId界面,直接在界面中处理
    if (bIsUIDWidget) {
        qInfo() << "当前界面是unoinId界面,直接在界面中处理";
        m_unionidWidget->getAccessToken(code,state);
    } else {//在Notificationmanager类中处理
        Notificationmanager::instance()->getAccessToken(code,state);
    }
}

void UnionidModule::onShowPopup(QString fileName)
{
    m_worker->requestAgreementPopup(fileName);
}
