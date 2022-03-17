/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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
#include "generalsettingwidget.h"
#include "mousesettingwidget.h"
#include "touchpadsettingwidget.h"
#include "trackpointsettingwidget.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"
#include "src/plugin-mouse/operation/mousedbusproxy.h"

#include <QDebug>
#include <QApplication>

DCC_USE_NAMESPACE

MouseModule::MouseModule(QObject *parent)
    : ModuleObject(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
{
    m_model  = new MouseModel();
    m_worker = new MouseWorker(m_model);
    m_dbusProxy = new MouseDBusProxy(m_worker);
    m_worker->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
    m_dbusProxy->moveToThread(qApp->thread());
    m_dbusProxy->active();
}

MouseModule::~MouseModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}



QString MousePlugin::name() const
{
    return QStringLiteral("mouse");
}

ModuleObject *MousePlugin::module()
{
    //一级菜单--鼠标与触摸板
    MouseModule *moduleInterface = new MouseModule(this);
    ModuleData *dataRoot = new ModuleData(this);
    dataRoot->Name = tr("Mouse");
    dataRoot->DisplayName = tr("Mouse");
    dataRoot->Description = tr("鼠标");
    dataRoot->Icon = QIcon::fromTheme("dcc_nav_mouse");
    moduleInterface->setModuleData(dataRoot);
    moduleInterface->setChildType(ModuleObject::ChildType::HList);

    //二级菜单--通用
    ModuleObject *moduleGeneral = new ModuleObject(tr("General"), tr("General"), this);
    moduleGeneral->setChildType(ModuleObject::ChildType::Page);
    GeneralSettingModule *generalSettingModule = new GeneralSettingModule(moduleInterface->model(), moduleInterface->work(), moduleGeneral);
    moduleGeneral->appendChild(generalSettingModule);
    moduleInterface->appendChild(moduleGeneral);


    //二级菜单--鼠标
    ModuleObject *moduleMouse = new ModuleObject(tr("Mouse"), tr("Mouse"), this);
    moduleMouse->setChildType(ModuleObject::ChildType::Page);
    MouseSettingModule *mouseSettingModule = new MouseSettingModule(moduleInterface->model(), moduleInterface->work(), moduleMouse);
    moduleMouse->appendChild(mouseSettingModule);
    moduleInterface->appendChild(moduleMouse);

    //二级菜单--触摸板
    ModuleObject *moduleTouchpad = new ModuleObject(tr("Touchpad"), tr("Touchpad"), this);
    moduleTouchpad->setChildType(ModuleObject::ChildType::Page);
    TouchPadSettingModule *touchPadSettingModule = new TouchPadSettingModule(moduleInterface->model(), moduleInterface->work(), moduleTouchpad);
    moduleTouchpad->appendChild(touchPadSettingModule);
    moduleInterface->appendChild(moduleTouchpad);

    //二级菜单--指点杆
    ModuleObject *moduleTrackPoint = new ModuleObject(tr("TrackPoint"), tr("TrackPoint"), this);
    moduleTrackPoint->setChildType(ModuleObject::ChildType::Page);
    TrackPointSettingModule *trackPointSettingModule = new TrackPointSettingModule(moduleInterface->model(), moduleInterface->work(), moduleTrackPoint);
    moduleTrackPoint->appendChild(trackPointSettingModule);
    moduleInterface->appendChild(moduleTrackPoint);

    return moduleInterface;
}


//三级菜单
QWidget *GeneralSettingModule::page()
{
    GeneralSettingWidget *w = new  GeneralSettingWidget;
    w->setModel(m_model);
    connect(w, &GeneralSettingWidget::requestSetLeftHand, m_worker, &MouseWorker::onLeftHandStateChanged);
    connect(w, &GeneralSettingWidget::requestSetDisTyping, m_worker, &MouseWorker::onDisTypingChanged);
    connect(w, &GeneralSettingWidget::requestScrollSpeed, m_worker, &MouseWorker::onScrollSpeedChanged);
    connect(w, &GeneralSettingWidget::requestSetDouClick, m_worker, &MouseWorker::onDouClickChanged);
    return w;
}

QWidget *TouchPadSettingModule::page()
{
    TouchPadSettingWidget *w = new  TouchPadSettingWidget;
    connect(w, &TouchPadSettingWidget::requestSetTouchpadMotionAcceleration, m_worker, &MouseWorker::onTouchpadMotionAccelerationChanged);
    connect(w, &TouchPadSettingWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
    connect(w, &TouchPadSettingWidget::requestSetTouchNaturalScroll, m_worker, &MouseWorker::onTouchNaturalScrollStateChanged);
    connect(w, &TouchPadSettingWidget::requestDetectState, m_worker, &MouseWorker::onPalmDetectChanged);
    connect(w, &TouchPadSettingWidget::requestContact, m_worker, &MouseWorker::onPalmMinWidthChanged);
    connect(w, &TouchPadSettingWidget::requestPressure, m_worker, &MouseWorker::onPalmMinzChanged);
    w->setModel(this->m_model);

    return w;
}

QWidget *TrackPointSettingModule::page()
{
    TrackPointSettingWidget *w = new  TrackPointSettingWidget;
    connect(w, &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration, m_worker, &MouseWorker::onTrackPointMotionAccelerationChanged);
    w->setModel(m_model);
    return w;
}

QWidget *MouseSettingModule::page()
{
    MouseSettingWidget *w = new  MouseSettingWidget;
    w->setModel(m_model);
    connect(w, &MouseSettingWidget::requestSetMouseMotionAcceleration, m_worker, &MouseWorker::onMouseMotionAccelerationChanged);
    connect(w, &MouseSettingWidget::requestSetAccelProfile, m_worker, &MouseWorker::onAccelProfileChanged);
    connect(w, &MouseSettingWidget::requestSetDisTouchPad, m_worker, &MouseWorker::onDisTouchPadChanged);
    connect(w, &MouseSettingWidget::requestSetMouseNaturalScroll, m_worker, &MouseWorker::onMouseNaturalScrollStateChanged);
    return w;
}
