// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wacommodule.h"

#include "window/modules/wacom/wacomwidget.h"

#include "window/modules/wacom/wacommodel.h"
#include "window/modules/wacom/wacomworker.h"

namespace DCC_NAMESPACE {
namespace wacom {
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

void WacomModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(sync);
    Q_UNUSED(pushtype);
    if (m_model) {
        delete m_model;
    }
    m_model  = new WacomModel(this);
    m_worker = new WacomWorker(m_model);

    setDeviceUnavailabel(!m_model->exist());

    connect(m_model, &WacomModel::existChanged, this, [this](const bool exist) {
        m_frameProxy->setModuleVisible(this, exist);
        setDeviceUnavailabel(!exist);
        initSearchData();
        m_frameProxy->updateSearchData(tr("Drawing Tablet"));
    });

    connect(m_model, &WacomModel::cursorModeChanged, this, [ = ] (const bool cursorMode) {
        Q_UNUSED(cursorMode);
        initSearchData();
        m_frameProxy->updateSearchData(tr("Drawing Tablet"));
    });

    setAvailable(m_model->exist());
    addChildPageTrans();
    initSearchData();
}

void WacomModule::initialize()
{
}

void WacomModule::active()
{
    m_worker->active();
    m_wacomWidget = new WacomWidget;
    m_wacomWidget->setVisible(false);
    connect(m_wacomWidget, &WacomWidget::requestSetPressureValue, m_worker, &WacomWorker::onPressureSensitiveChanged);
    connect(m_wacomWidget, &WacomWidget::modeChanged, m_worker, &WacomWorker::setCursorMode);

    m_wacomWidget->setModel(m_model);
    setDeviceUnavailabel(!m_model->exist());
    m_frameProxy->pushWidget(this, m_wacomWidget);
    m_wacomWidget->setVisible(true);
}

void WacomModule::deactive()
{
    m_worker->deactive();
}

const QString WacomModule::name() const
{
    return QStringLiteral("wacom");
}

const QString WacomModule::displayName() const
{
    return tr("Drawing Tablet");
}

QStringList WacomModule::availPage() const
{
    QStringList sl;
    sl << "Pressure Sensitivity" << "Pen" << "Mouse";

    return sl;
}

void WacomModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //wacom
        m_frameProxy->addChildPageTrans("Pressure Sensitivity", tr("Pressure Sensitivity"));
        m_frameProxy->addChildPageTrans("Mode", tr("Mode"));
    }
}

void WacomModule::initSearchData()
{
    //todo
    auto func_process_all = [ = ] {
        const QString& module = displayName();
        m_frameProxy->setModuleVisible(module, m_model->exist());
        m_frameProxy->setWidgetVisible(module, tr("Mode"), m_model->exist());
        m_frameProxy->setWidgetVisible(module, tr("Pressure Sensitivity"), m_model->exist() && !m_model->getCursorMode());
     };

    func_process_all();
}
}
}
