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
#include "interface/pagemodule.h"
#include "mousesettingwidget.h"
#include "src/plugin-mouse/operation/mousedbusproxy.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"
#include "touchpadsettingwidget.h"
#include "trackpointsettingwidget.h"

#include <QApplication>
#include <QDebug>

using namespace DCC_NAMESPACE;

MouseModule::MouseModule(QObject *parent)
    : HListModule(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
{
    m_model = new MouseModel(this);
    m_worker = new MouseWorker(m_model, this);
    m_dbusProxy = new MouseDBusProxy(m_worker, this);
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
    // 一级菜单--鼠标与触摸板
    MouseModule *moduleInterface = new MouseModule();
    moduleInterface->setName("mouse");
    moduleInterface->setDisplayName(tr("Mouse"));
    moduleInterface->setDescription(tr("Mouse"));
    moduleInterface->setIcon(QIcon::fromTheme("dcc_nav_mouse"));

    // 二级菜单--通用
    ModuleObject *moduleGeneral = new PageModule("mouseGeneral", tr("General"));
    GeneralSettingModule *generalSettingModule = new GeneralSettingModule(moduleInterface->model(),
                                                                          moduleInterface->work(),
                                                                          moduleGeneral);
    moduleGeneral->appendChild(generalSettingModule);
    moduleInterface->appendChild(moduleGeneral);

    // 二级菜单--鼠标
    ModuleObject *moduleMouse = new PageModule("mouseMouse", tr("Mouse"));
    MouseSettingModule *mouseSettingModule =
            new MouseSettingModule(moduleInterface->model(), moduleInterface->work(), moduleMouse);
    moduleMouse->appendChild(mouseSettingModule);
    moduleInterface->appendChild(moduleMouse);

    // 二级菜单--触摸板
    ModuleObject *moduleTouchpad = new PageModule("mouseTouch", tr("Touchpad"));
    TouchPadSettingModule *touchPadSettingModule =
            new TouchPadSettingModule(moduleInterface->model(),
                                      moduleInterface->work(),
                                      moduleTouchpad);
    moduleTouchpad->appendChild(touchPadSettingModule);
    moduleInterface->appendChild(moduleTouchpad);

    // 二级菜单--指点杆
    ModuleObject *moduleTrackPoint = new PageModule("mouseTrackpoint", tr("TrackPoint"));
    TrackPointSettingModule *trackPointSettingModule =
            new TrackPointSettingModule(moduleInterface->model(),
                                        moduleInterface->work(),
                                        moduleTrackPoint);
    moduleTrackPoint->appendChild(trackPointSettingModule);
    moduleInterface->appendChild(moduleTrackPoint);

    return moduleInterface;
}

QString MousePlugin::location() const
{
    return "11";
}

// 三级菜单
QWidget *GeneralSettingModule::page()
{
    GeneralSettingWidget *w = new GeneralSettingWidget;
    w->setModel(m_model);
    connect(w,
            &GeneralSettingWidget::requestSetLeftHand,
            m_worker,
            &MouseWorker::onLeftHandStateChanged);
    connect(w,
            &GeneralSettingWidget::requestSetDisTyping,
            m_worker,
            &MouseWorker::onDisTypingChanged);
    connect(w,
            &GeneralSettingWidget::requestScrollSpeed,
            m_worker,
            &MouseWorker::onScrollSpeedChanged);
    connect(w,
            &GeneralSettingWidget::requestSetDouClick,
            m_worker,
            &MouseWorker::onDouClickChanged);
    return w;
}

TouchPadSettingModule::TouchPadSettingModule(MouseModel *model,
                                             MouseWorker *worker,
                                             ModuleObject *parent)
    : ModuleObject(parent)
    , m_model(model)
    , m_worker(worker)
{
    parent->setHidden(!m_model->tpadExist());
    connect(m_model, &MouseModel::tpadExistChanged, parent, [parent](bool tpadExist) {
        parent->setHidden(!tpadExist);
    });
}

QWidget *TouchPadSettingModule::page()
{
    TouchPadSettingWidget *w = new TouchPadSettingWidget;
    connect(w,
            &TouchPadSettingWidget::requestSetTouchpadMotionAcceleration,
            m_worker,
            &MouseWorker::onTouchpadMotionAccelerationChanged);
    connect(w, &TouchPadSettingWidget::requestSetTapClick, m_worker, &MouseWorker::onTapClick);
    connect(w,
            &TouchPadSettingWidget::requestSetTouchNaturalScroll,
            m_worker,
            &MouseWorker::onTouchNaturalScrollStateChanged);
    connect(w,
            &TouchPadSettingWidget::requestDetectState,
            m_worker,
            &MouseWorker::onPalmDetectChanged);
    connect(w,
            &TouchPadSettingWidget::requestContact,
            m_worker,
            &MouseWorker::onPalmMinWidthChanged);
    connect(w, &TouchPadSettingWidget::requestPressure, m_worker, &MouseWorker::onPalmMinzChanged);
    w->setModel(this->m_model);

    return w;
}

TrackPointSettingModule::TrackPointSettingModule(MouseModel *model,
                                                 MouseWorker *worker,
                                                 ModuleObject *parent)
    : ModuleObject(parent)
    , m_model(model)
    , m_worker(worker)
{
    parent->setHidden(!m_model->redPointExist());
    connect(m_model, &MouseModel::redPointExistChanged, parent, [parent](bool rPointExist) {
        parent->setHidden(!rPointExist);
    });
}

QWidget *TrackPointSettingModule::page()
{
    TrackPointSettingWidget *w = new TrackPointSettingWidget;
    connect(w,
            &TrackPointSettingWidget::requestSetTrackPointMotionAcceleration,
            m_worker,
            &MouseWorker::onTrackPointMotionAccelerationChanged);
    w->setModel(m_model);
    return w;
}

QWidget *MouseSettingModule::page()
{
    MouseSettingWidget *w = new MouseSettingWidget;
    w->setModel(m_model);
    connect(w,
            &MouseSettingWidget::requestSetMouseMotionAcceleration,
            m_worker,
            &MouseWorker::onMouseMotionAccelerationChanged);
    connect(w,
            &MouseSettingWidget::requestSetAccelProfile,
            m_worker,
            &MouseWorker::onAccelProfileChanged);
    connect(w,
            &MouseSettingWidget::requestSetDisTouchPad,
            m_worker,
            &MouseWorker::onDisTouchPadChanged);
    connect(w,
            &MouseSettingWidget::requestSetMouseNaturalScroll,
            m_worker,
            &MouseWorker::onMouseNaturalScrollStateChanged);
    return w;
}
