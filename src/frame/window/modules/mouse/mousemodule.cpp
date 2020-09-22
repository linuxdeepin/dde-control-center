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
#include "QDebug"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;

MouseModule::MouseModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_mouseWidget(nullptr)
    , m_generalSettingWidget(nullptr)
    , m_mouseSettingWidget(nullptr)
    , m_touchpadSettingWidget(nullptr)
    , m_trackPointSettingWidget(nullptr)
{
}

void MouseModule::preInitialize(bool sync)
{
    //添加此判断是因为公共功能可能泄露。在分配指针“m_model”之前未释放它
    if (m_model) {
        delete m_model;
    }
    m_model  = new dcc::mouse::MouseModel(this);
    m_worker = new dcc::mouse::MouseWorker(m_model, this);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
    m_worker->active();

    connect(m_model, &MouseModel::tpadExistChanged, this, [this](bool state) {
        qDebug() << "[Mouse] Touchpad , exist state : " << state;
        m_frameProxy->setRemoveableDeviceStatus(tr("Touchpad"), state);
    });//触控板
    connect(m_model, &MouseModel::redPointExistChanged, this, [this](bool state) {
        qDebug() << "[Mouse] TrackPoint , exist state : " << state;
        m_frameProxy->setRemoveableDeviceStatus(tr("TrackPoint"), state);
    });//指点杆
}

void MouseModule::initialize()
{

}

void MouseModule::reset()
{
    m_worker->onDefaultReset();
}

void MouseModule::active()
{
    m_mouseWidget = new MouseWidget;
    m_mouseWidget->setVisible(false);
    m_mouseWidget->init(m_model->tpadExist(), m_model->redPointExist());
    connect(m_model, &MouseModel::tpadExistChanged, m_mouseWidget, &MouseWidget::tpadExistChanged);
    connect(m_model, &MouseModel::redPointExistChanged, m_mouseWidget, &MouseWidget::redPointExistChanged);
    connect(m_mouseWidget, &MouseWidget::showGeneralSetting, this, &MouseModule::showGeneralSetting);
    connect(m_mouseWidget, &MouseWidget::showMouseSetting, this, &MouseModule::showMouseSetting);
    connect(m_mouseWidget, &MouseWidget::showTouchpadSetting, this, &MouseModule::showTouchpadSetting);
    connect(m_mouseWidget, &MouseWidget::showTrackPointSetting, this, &MouseModule::showTrackPointSetting);
    m_frameProxy->pushWidget(this, m_mouseWidget);
    m_mouseWidget->setVisible(true);
    showGeneralSetting();
}

void MouseModule::showGeneralSetting()
{
    m_generalSettingWidget = new GeneralSettingWidget;
    m_generalSettingWidget->setVisible(false);
    m_generalSettingWidget->setModel(m_model);

    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetLeftHand, m_worker, &MouseWorker::onLeftHandStateChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetDisTyping, m_worker, &MouseWorker::onDisTypingChanged);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestScrollSpeed, m_worker, &MouseWorker::setScrollSpeed);
    connect(m_generalSettingWidget, &GeneralSettingWidget::requestSetDouClick, m_worker, &MouseWorker::onDouClickChanged);

    m_frameProxy->pushWidget(this, m_generalSettingWidget);
    m_generalSettingWidget->setVisible(true);
}

void MouseModule::showMouseSetting()
{
    m_mouseSettingWidget = new MouseSettingWidget;
    m_mouseSettingWidget->setVisible(false);
    m_mouseSettingWidget->setModel(m_model);

    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetMouseMotionAcceleration, m_worker, &MouseWorker::onMouseMotionAccelerationChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetAccelProfile, m_worker, &MouseWorker::onAccelProfileChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetDisTouchPad, m_worker, &MouseWorker::onDisTouchPadChanged);
    connect(m_mouseSettingWidget, &MouseSettingWidget::requestSetMouseNaturalScroll, m_worker, &MouseWorker::onMouseNaturalScrollStateChanged);

    m_frameProxy->pushWidget(this, m_mouseSettingWidget);
    m_mouseSettingWidget->setVisible(true);
}

void MouseModule::showTouchpadSetting()
{
    m_touchpadSettingWidget = new TouchPadSettingWidget;
    m_touchpadSettingWidget->setVisible(false);
    m_touchpadSettingWidget->setModel(m_model);

    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTouchpadMotionAcceleration, m_worker, &MouseWorker::onTouchpadMotionAccelerationChanged);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestSetTouchNaturalScroll, m_worker, &MouseWorker::onTouchNaturalScrollStateChanged);

    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestDetectState, m_worker, &MouseWorker::setPalmDetect);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestContact, m_worker, &MouseWorker::setPalmMinWidth);
    connect(m_touchpadSettingWidget, &TouchPadSettingWidget::requestPressure, m_worker, &MouseWorker::setPalmMinz);

    m_frameProxy->pushWidget(this, m_touchpadSettingWidget);
    m_touchpadSettingWidget->setVisible(true);
}

void MouseModule::showTrackPointSetting()
{
    m_trackPointSettingWidget = new TrackPointSettingWidget;
    m_trackPointSettingWidget->setVisible(false);
    m_trackPointSettingWidget->setModel(m_model);
    connect(m_trackPointSettingWidget, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration, m_worker, &MouseWorker::onTrackPointMotionAccelerationChanged);
    m_frameProxy->pushWidget(this, m_trackPointSettingWidget);
    m_trackPointSettingWidget->setVisible(true);
}

const QString MouseModule::name() const
{
    return QStringLiteral("mouse");
}

const QString MouseModule::displayName() const
{
    return tr("Mouse");
}

int MouseModule::load(const QString &path)
{
    int hasPage = -1;
    QString loadPath = path.split("/").at(0);
    if (loadPath == QStringLiteral("General")) {
        hasPage = 0;
        m_mouseWidget->initSetting(0);
    } else if (loadPath == QStringLiteral("Mouse")) {
        hasPage = 0;
        m_mouseWidget->initSetting(1);
    } else if (loadPath == QStringLiteral("Touchpad")) {
        hasPage = 0;
        m_mouseWidget->initSetting(2);
    } else if (loadPath == QStringLiteral("TrackPoint")) {
        hasPage = 0;
        m_mouseWidget->initSetting(3);
    }

    return hasPage;
}

QStringList MouseModule::availPage() const
{
    QStringList sl;
    sl << "General" << "Mouse";
    if (m_model->tpadExist()) {
        sl << "Touchpad";
    }
    if (m_model->redPointExist()) {
        sl << "TrackPoint";
    }
    return sl;
}

void MouseModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
