/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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
#include "generalsettingwidget.h"
#include "mousesettingwidget.h"
#include "touchpadsettingwidget.h"
#include "trackpointsettingwidget.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/mouseworker.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;

MouseModule::MouseModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_generalSettingWidget(nullptr)
    , m_mouseSettingWidget(nullptr)
    , m_touchpadSettingWidget(nullptr)
    , m_trackPointSettingWidget(nullptr)
{
}

void MouseModule::initialize()
{
    m_model  = new dcc::mouse::MouseModel(this);
    m_worker = new dcc::mouse::MouseWorker(m_model, this);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
    m_worker->init();
}

void MouseModule::reset()
{
    m_worker->onDefaultReset();
}

void MouseModule::active()
{
    MouseWidget *mouseWidget = new MouseWidget;
    connect(mouseWidget, &MouseWidget::showGeneralSetting, this, &MouseModule::showGeneralSetting);
    connect(mouseWidget, &MouseWidget::showMouseSetting, this, &MouseModule::showMouseSetting);
    connect(mouseWidget, &MouseWidget::showTouchpadSetting, this, &MouseModule::showTouchpadSetting);
    connect(mouseWidget, &MouseWidget::showTrackPointSetting, this, &MouseModule::showTrackPointSetting);
    m_frameProxy->pushWidget(this, mouseWidget);
    showGeneralSetting();
}

void MouseModule::showGeneralSetting()
{
    m_generalSettingWidget = new GeneralSettingWidget;
    m_generalSettingWidget->setModel(m_model);

    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetLeftHand, m_worker, &MouseWorker::onLeftHandStateChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetDisTyping, m_worker, &MouseWorker::onDisTypingChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestScrollSpeed, m_worker, &MouseWorker::setScrollSpeed);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetDouClick, m_worker, &MouseWorker::onDouClickChanged);

    m_frameProxy->pushWidget(this, m_generalSettingWidget);
}

void MouseModule::showMouseSetting()
{
    m_mouseSettingWidget = new MouseSettingWidget;
    m_mouseSettingWidget->setModel(m_model);

    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetMouseMotionAcceleration, m_worker, &MouseWorker::onMouseMotionAccelerationChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetAccelProfile, m_worker, &MouseWorker::onAccelProfileChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetDisTouchPad, m_worker, &MouseWorker::onDisTouchPadChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetMouseNaturalScroll, m_worker, &MouseWorker::onMouseNaturalScrollStateChanged);

    m_frameProxy->pushWidget(this, m_mouseSettingWidget);
}

void MouseModule::showTouchpadSetting()
{
    m_touchpadSettingWidget = new TouchPadSettingWidget;
    m_touchpadSettingWidget->setModel(m_model);

    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTouchpadMotionAcceleration, m_worker, &MouseWorker::onTouchpadMotionAccelerationChanged);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTouchNaturalScroll, m_worker, &MouseWorker::onTouchNaturalScrollStateChanged);

    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestDetectState, m_worker, &MouseWorker::setPalmDetect);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestContact, m_worker, &MouseWorker::setPalmMinWidth);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestPressure, m_worker, &MouseWorker::setPalmMinz);

    m_frameProxy->pushWidget(this, m_touchpadSettingWidget);
}

void MouseModule::showTrackPointSetting()
{
    m_trackPointSettingWidget = new TrackPointSettingWidget;
    m_trackPointSettingWidget->setModel(m_model);
    connect(m_trackPointSettingWidget, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration, m_worker, &MouseWorker::onTrackPointMotionAccelerationChanged);
    m_frameProxy->pushWidget(this, m_trackPointSettingWidget);
}

const QString MouseModule::name() const
{
    return QStringLiteral("mouse");
}

void MouseModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
