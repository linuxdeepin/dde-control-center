// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "touchscreenmodule.h"
#include "touchscreenwidget.h"
#include "modules/display/monitor.h"
#include "modules/display/recognizewidget.h"
#include "modules/touchscreen/touchscreenmodel.h"
#include "modules/touchscreen/touchscreenworker.h"

using namespace dcc::display;
using namespace DCC_NAMESPACE::touchscreen;

TouchscreenModule::TouchscreenModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_touchscreenModel(nullptr)
    , m_touchscreenWorker(nullptr)
    , m_touchscreenWidget(nullptr)
{
}

TouchscreenModule::~TouchscreenModule()
{
    m_touchscreenModel->deleteLater();
    m_touchscreenWorker->deleteLater();
}

void TouchscreenModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    if (m_touchscreenModel) {
        delete m_touchscreenModel;
    }
    m_touchscreenModel = new TouchscreenModel;
    m_touchscreenWorker = new TouchscreenWorker(m_touchscreenModel, nullptr, sync);

    m_touchscreenModel->moveToThread(qApp->thread());
    m_touchscreenWorker->moveToThread(qApp->thread());

    addChildPageTrans();

    m_touchscreenWorker->active();
    initSearchData();
}

void TouchscreenModule::initialize()
{

}

void TouchscreenModule::active()
{
    m_touchscreenWidget = new TouchscreenWidget;
    m_touchscreenWidget->setVisible(false);

    connect(m_touchscreenWidget, &TouchscreenWidget::requestAssociateTouch, m_touchscreenWorker, &TouchscreenWorker::setTouchScreenAssociation);
    m_touchscreenWidget->setModel(m_touchscreenModel);
    m_frameProxy->pushWidget(this, m_touchscreenWidget);
    m_touchscreenWidget->setVisible(true);
}

const QString TouchscreenModule::name() const
{
    return QStringLiteral("touchscreen");
}

const QString TouchscreenModule::displayName() const
{
    return tr("Touch Screen");
}

QStringList TouchscreenModule::availPage() const
{
    QStringList sl;
    if (m_touchscreenModel && !m_touchscreenModel->touchscreenList().isEmpty()) {
        sl << "Touch Screen";
    }

    return sl;
}

void TouchscreenModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        m_frameProxy->addChildPageTrans("Select your touch screen when connected or set it here.", tr("Select your touch screen when connected or set it here."));
    }
}

void TouchscreenModule::showTouchRecognize()
{
    // 只有一块屏幕不显示触摸提示
    if (m_touchscreenModel->touchscreenList().size() < 2) {
        return;
    }

    // 复制模式
    if (m_touchscreenModel->displayMode() == 1) {
        QString text = m_touchscreenModel->monitorList().first()->name();
        for (int i = 1; i < m_touchscreenModel->monitorList().size(); i++) {
            text += QString(" = %1").arg(m_touchscreenModel->monitorList()[i]->name());
        }

        // 所在显示器不存在显示框
        if (m_recognizeWidget.value(text) == nullptr) {
            RecognizeWidget *widget = new RecognizeWidget(m_touchscreenModel->monitorList()[0], text);
            QTimer::singleShot(5000, this, [=] {
                widget->deleteLater();
                m_recognizeWidget.remove(text);
            });
            m_recognizeWidget[text] = widget;
        }
    } else { // 扩展模式
        for (auto monitor : m_touchscreenModel->monitorList()) {
            // 所在显示器不存在显示框
            if (m_recognizeWidget.value(monitor->name()) == nullptr) {
                RecognizeWidget *widget = new RecognizeWidget(monitor, monitor->name());
                m_recognizeWidget[monitor->name()] = widget;
                QTimer::singleShot(5000, this, [=] {
                    widget->deleteLater();
                    m_recognizeWidget.remove(monitor->name());
                });
                m_recognizeWidget[monitor->name()] = widget;
            }
        }
    }
}

void TouchscreenModule::initSearchData()
{
    auto func_process_all = [this] {
        const QString& module = displayName();
        const bool visible = !m_touchscreenModel->touchscreenList().isEmpty();
        m_frameProxy->setModuleVisible(module, visible);
        setDeviceUnavailabel(!visible);
        m_frameProxy->setWidgetVisible(module, tr("Select your touch screen when connected or set it here."), visible);
     };

    connect(m_touchscreenModel, &TouchscreenModel::touchscreenListChanged, this, [=] {
        func_process_all();
        showTouchRecognize();
    });

    func_process_all();
}
