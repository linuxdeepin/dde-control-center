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
#include "unionidwidget.h"
#include "window/utils.h"
#include "window/mainwindow.h"

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
    return QStringLiteral("unionid");
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
    UnionidWidget *widget = new UnionidWidget;
    connect(widget, &UnionidWidget::requestLoginUser, m_worker, &dcc::unionid::UnionidWorker::loginUser, Qt::UniqueConnection);
    connect(widget, &UnionidWidget::requestSetAutoSync, m_worker, &dcc::unionid::UnionidWorker::setAutoSync, Qt::UniqueConnection);
    connect(widget, &UnionidWidget::requestLogoutUser, m_worker, &dcc::unionid::UnionidWorker::logoutUser, Qt::QueuedConnection);
    connect(widget, &UnionidWidget::requesUserDialog, m_worker, &dcc::unionid::UnionidWorker::requestModifyDialog, Qt::UniqueConnection);
    connect(widget, &UnionidWidget::requestPopupDialog, this, &UnionidModule::onShowPopup, Qt::UniqueConnection);
    widget->setModel(m_model);
    m_worker->activate(); //refresh data

    m_frameProxy->pushWidget(this, widget);
}

void UnionidModule::preInitialize(bool sync)
{
    Q_UNUSED(sync);
    m_model = new dcc::unionid::UnionidModel;
    m_worker = new dcc::unionid::UnionidWorker(m_model);

    m_frameProxy->setModuleVisible(this, m_model->syncIsValid() && !IsServerSystem && false);
}

QStringList UnionidModule::availPage() const
{
    QStringList sl;
    sl << "unionid " << "Unionid Sign In";
    return sl;
}

void UnionidModule::onShowPopup(QString fileName)
{
    m_worker->requestAgreementPopup(fileName);
}
