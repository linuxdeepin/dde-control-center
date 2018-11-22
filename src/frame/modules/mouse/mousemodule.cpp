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

#include "mousemodule.h"
#include "mousewidget.h"
#include "mousemodel.h"
#include "mouseworker.h"
#include "widgets/contentwidget.h"
using namespace dcc;
using namespace dcc::mouse;
MouseModule::MouseModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_mouseWidget(nullptr)
{
}

void MouseModule::initialize()
{
    m_model  = new MouseModel;
    m_worker = new MouseWorker(m_model);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

void MouseModule::moduleActive()
{
    m_worker->active();
}

void MouseModule::moduleDeactive()
{
    m_worker->deactive();
}

void MouseModule::reset()
{
    m_worker->onDefaultReset();
}

ModuleWidget *MouseModule::moduleWidget()
{
    if (!m_mouseWidget) {
        m_mouseWidget = new MouseWidget;
        m_mouseWidget->setModel(m_model);
        connect(m_mouseWidget, &MouseWidget::requestSetLeftHand, m_worker, &MouseWorker::onLeftHandStateChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetMouseNaturalScroll, m_worker, &MouseWorker::onMouseNaturalScrollStateChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTouchNaturalScroll, m_worker, &MouseWorker::onTouchNaturalScrollStateChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetDisTyping, m_worker, &MouseWorker::onDisTypingChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetDisTouchPad, m_worker, &MouseWorker::onDisTouchPadChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
        connect(m_mouseWidget, &MouseWidget::setDefault, m_worker, &MouseWorker::onDefaultReset);
        connect(m_mouseWidget, &MouseWidget::requestSetDouClick, m_worker, &MouseWorker::onDouClickChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetMouseMotionAcceleration, m_worker, &MouseWorker::onMouseMotionAccelerationChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetAccelProfile, m_worker, &MouseWorker::onAccelProfileChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTouchpadMotionAcceleration, m_worker, &MouseWorker::onTouchpadMotionAccelerationChanged);
        connect(m_mouseWidget, &MouseWidget::requestSetTrackPointMotionAcceleration, m_worker, &MouseWorker::onTrackPointMotionAccelerationChanged);
        connect(m_mouseWidget, &MouseWidget::requestDetectState, m_worker, &MouseWorker::setPalmDetect);
        connect(m_mouseWidget, &MouseWidget::requestContact, m_worker, &MouseWorker::setPalmMinWidth);
        connect(m_mouseWidget, &MouseWidget::requestPressure, m_worker, &MouseWorker::setPalmMinz);
        connect(m_mouseWidget, &MouseWidget::requestScrollSpeed, m_worker, &MouseWorker::setScrollSpeed);
    }
    return m_mouseWidget;
}

const QString MouseModule::name() const
{
    return QStringLiteral("mouse");
}

MouseModule::~MouseModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void MouseModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
