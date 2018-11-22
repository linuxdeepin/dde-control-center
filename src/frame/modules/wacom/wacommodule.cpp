/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "wacommodule.h"
#include "widgets/contentwidget.h"
#include "wacommodel.h"
#include "wacomwidget.h"
#include "wacomworker.h"
#include "wacommodepage.h"

using namespace dcc;
using namespace dcc::wacom;
WacomModule::WacomModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_wacomWidget(nullptr)
{
}

void WacomModule::initialize()
{
    m_model  = new WacomModel;
    m_worker = new WacomWorker(m_model);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

void WacomModule::moduleActive()
{
    m_worker->active();
}

void WacomModule::moduleDeactive()
{
    m_worker->deactive();
}

void WacomModule::reset()
{

}

ModuleWidget *WacomModule::moduleWidget()
{
    if (!m_wacomWidget)
    {
        m_wacomWidget = new WacomWidget;

        connect(m_wacomWidget, &WacomWidget::requestSetPressureValue, m_worker, &WacomWorker::onPressureSensitiveChanged);
        connect(m_model, &WacomModel::existChanged, [this](const bool exist) { m_frameProxy->setModuleVisible(this, exist); });
        connect(m_wacomWidget, &WacomWidget::requestShowMode, this, &WacomModule::showModePage);

        m_wacomWidget->setModel(m_model);
        m_frameProxy->setModuleVisible(this, m_model->exist());
    }

    return m_wacomWidget;
}

const QString WacomModule::name() const
{
    return QStringLiteral("wacom");
}

void WacomModule::showModePage()
{
    WacomModePage *page = new WacomModePage;
    page->setMode(m_model->getCursorMode());

    connect(page, &WacomModePage::requestSetMode, m_worker, &WacomWorker::setCursorMode);

    m_frameProxy->pushWidget(this, page);
}

WacomModule::~WacomModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void WacomModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
