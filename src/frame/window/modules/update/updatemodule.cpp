/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "updatemodule.h"
#include "modules/update/updatemodel.h"
#include "modules/update/updatework.h"
#include "updatewidget.h"
#include "modules/update/mirrorswidget.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateModule::UpdateModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_work(nullptr)
{

}

void UpdateModule::initialize()
{
    m_model = new UpdateModel;
    m_work = new UpdateWorker(m_model);
    m_work->activate(); //refresh data

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

void UpdateModule::reset()
{

}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

void UpdateModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)
}

QWidget *UpdateModule::moduleWidget()
{
    UpdateWidget *mainWidget = new UpdateWidget;
    mainWidget->initialize();
    mainWidget->setModel(m_model, m_work);

    connect(mainWidget, &UpdateWidget::pushMirrorsView, this, [this]() {
        if (!m_mirrorsWidget) {
            m_mirrorsWidget = new MirrorsWidget(m_model);

            m_work->checkNetselect();

            connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWorker::setMirrorSource);
            connect(m_mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work, &UpdateWorker::testMirrorSpeed);
        }

        m_frameProxy->pushWidget(this, m_mirrorsWidget);
    });

    return mainWidget;
}

void UpdateModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w)
}
