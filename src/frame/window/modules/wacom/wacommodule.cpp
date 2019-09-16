/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wp <wangpeng_cm@deepin.com>
 *
 * Maintainer: wp <wangpeng_cm@deepin.com>
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

#include "window/modules/wacom/wacomwidget.h"

#include "modules/wacom/wacommodel.h"
#include "modules/wacom/wacomworker.h"
#include "modules/wacom/wacommodepage.h"

using namespace dcc::wacom;
using namespace DCC_NAMESPACE::wacom;
WacomModule::WacomModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_wacomWidget(nullptr)
    , m_model(nullptr)
    , m_worker(nullptr)
{
}

WacomModule::~WacomModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();

    if (m_wacomWidget) {
        m_wacomWidget->deleteLater();
    }
}

void WacomModule::initialize()
{
    m_model  = new WacomModel(this);
    m_worker = new WacomWorker(m_model);

    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());

    m_frameProxy->setModuleVisible(this, m_model->exist());

    connect(m_model, &WacomModel::existChanged, this, [this](const bool exist) {
        m_frameProxy->setModuleVisible(this, exist);

        if (!exist) {
            m_frameProxy->popWidget(this);
        }
    });
}

void WacomModule::active()
{
    m_worker->active();
    m_wacomWidget = new WacomWidget;

    connect(m_wacomWidget, &WacomWidget::requestSetPressureValue, m_worker, &WacomWorker::onPressureSensitiveChanged);
    connect(m_wacomWidget, &WacomWidget::requestShowMode, this, &WacomModule::showModePage);

    m_wacomWidget->setModel(m_model);
    m_frameProxy->setModuleVisible(this, m_model->exist());
    m_frameProxy->pushWidget(this, m_wacomWidget);
}

void WacomModule::deactive()
{
    m_worker->deactive();
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
