/*
 * Copyright (C) 2011 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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

    bool visible = !m_touchscreenModel->touchscreenList().isEmpty();
    m_frameProxy->setModuleVisible(this, visible);
    setDeviceUnavailabel(!visible);
    connect(m_touchscreenModel, &TouchscreenModel::touchscreenListChanged, this, [=] {
        bool visible = !m_touchscreenModel->touchscreenList().isEmpty();
        m_frameProxy->setModuleVisible(this, visible);
        setDeviceUnavailabel(!visible);
        showTouchRecognize();
    });

    addChildPageTrans();

    m_touchscreenWorker->active();
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
        m_frameProxy->addChildPageTrans("", "");
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

}
