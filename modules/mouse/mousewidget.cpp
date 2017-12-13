/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "mousewidget.h"
#include "switchwidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "widget/basesettings.h"
#include  "widget/mousesettings.h"
#include "widget/thinkpadsettings.h"
#include "mousemodel.h"
#include "mouse/model/mousemodelmousesettings.h"
#include "widget/palmdetectsetting.h"

#include "widget/doutestwidget.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;
using namespace dcc::mouse;
using namespace dcc::widgets;
MouseWidget::MouseWidget()
    : ModuleWidget()
{
    setObjectName("Mouse");

    m_baseSettings = new BaseSettings;
    m_centralLayout->addWidget(m_baseSettings);

    m_mouseSettings = new MouseSettings(tr("Mouse"));
    m_mouseSettings->setSwitchTitle(QString(tr("Disable the touchpad when inserting the mouse")));
    m_centralLayout->addWidget(m_mouseSettings);


    m_touchSettings = new MouseSettings(tr("TouchPad"));
    m_touchSettings->setSwitchTitle(QString(tr("Tap to Click")));
    m_centralLayout->addWidget(m_touchSettings);

    m_palmDetectSetting = new PalmDetectSetting;
    m_centralLayout->addWidget(m_palmDetectSetting);

    m_ThinkapdSettings = new ThinkpadSettings;
    m_centralLayout->addWidget(m_ThinkapdSettings);

    connect(m_baseSettings, &BaseSettings::requestSetLeftHand, this, &MouseWidget::requestSetLeftHand);
    connect(m_baseSettings, &BaseSettings::requestSetDisTyping, this, &MouseWidget::requestSetDisTyping);
    connect(m_mouseSettings, &MouseSettings::requestSetSwitch, this, &MouseWidget::requestSetDisTouchPad);
    connect(m_touchSettings, &MouseSettings::requestSetSwitch, this, &MouseWidget::requestSetTapClick);

    connect(m_baseSettings, &BaseSettings::requestSetSliderValue, this, &MouseWidget::requestSetDouClick);
    connect(m_mouseSettings, &MouseSettings::requestSetSliderValue, this, &MouseWidget::requestSetMouseMotionAcceleration);
    connect(m_touchSettings, &MouseSettings::requestSetSliderValue, this, &MouseWidget::requestSetTouchpadMotionAcceleration);
    connect(m_ThinkapdSettings, &ThinkpadSettings::requestSetSliderValue, this, &MouseWidget::requestSetTrackPointMotionAcceleration);

    connect(m_mouseSettings, &MouseSettings::requestSetNaturalScroll, this, &MouseWidget::requestSetMouseNaturalScroll);
    connect(m_touchSettings, &MouseSettings::requestSetNaturalScroll, this, &MouseWidget::requestSetTouchNaturalScroll);

    connect(m_palmDetectSetting, &PalmDetectSetting::requestDetectState, this, &MouseWidget::requestDetectState);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestContact, this, &MouseWidget::requestContact);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestPressure, this, &MouseWidget::requestPressure);

    setTitle(tr("Mouse and Touchpad"));

    m_baseSettingsGrp = new SettingsGroup;
    m_mouseSettingsGrp = new SettingsGroup(tr("Mouse"));
    m_touchSettingsGrp = new SettingsGroup(tr("TouchPad"));
    m_ThinkapdSettingsGrp = new SettingsGroup(tr("TrackPoint"));

    m_leftHand = new SwitchWidget(tr("Left Hand"));
    m_disInTyping = new SwitchWidget(tr("Disable the touchpad while typing"));
    m_doubleSlider = new TitledSliderItem(tr("Double-click Speed"));
    m_doubleTest = new DouTestWidget;

    m_mouseMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_disTchStn = new SwitchWidget(tr("Disable the touchpad when inserting the mouse"));
    m_mouseNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));

    m_touchMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_touchClickStn = new SwitchWidget(tr("Tap to Click"));
    m_touchNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));

    m_trackMoveSlider = new TitledSliderItem(tr("Pointer Speed"));

    m_baseSettingsGrp->appendItem(m_leftHand);
    m_baseSettingsGrp->appendItem(m_disInTyping);
    m_baseSettingsGrp->appendItem(m_doubleSlider);
    m_baseSettingsGrp->appendItem(m_doubleTest);

    m_mouseSettingsGrp->appendItem(m_mouseMoveSlider);
    m_mouseSettingsGrp->appendItem(m_disTchStn);
    m_mouseSettingsGrp->appendItem(m_mouseNaturalScroll);

    m_touchSettingsGrp->appendItem(m_touchMoveSlider);
    m_touchSettingsGrp->appendItem(m_touchClickStn);
    m_touchSettingsGrp->appendItem(m_touchNaturalScroll);

    m_ThinkapdSettingsGrp->appendItem(m_trackMoveSlider);
}

void MouseWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;

    m_baseSettings->setModel(model->getBaseSettings());
    m_mouseSettings->setModel(model->getMouseSettings());
    m_touchSettings->setModel(model->getTouchSettings());
    m_ThinkapdSettings->setModel(model->getTrackSettings());
    m_palmDetectSetting->setModel(model);

    m_touchpadModel = model->getTouchSettings();

    MouseModelMouseSettings *baseSettings = model->getTouchSettings();
    connect(baseSettings, &MouseModelMouseSettings::existChanged, this, &MouseWidget::onTouchpadVisibleChanged);

    onTouchpadVisibleChanged(baseSettings->getExist());

    MouseModelMouseSettings *mouseModel = model->getMouseSettings();
    connect(mouseModel, &MouseModelMouseSettings::switchChanged, this, &MouseWidget::onTouchpadHideChanged);
    connect(mouseModel, &MouseModelMouseSettings::existChanged, this, &MouseWidget::onTouchpadHideChanged);

    onTouchpadHideChanged(mouseModel->getSwitchState());
}

void MouseWidget::onTouchpadVisibleChanged(const bool visible)
{
    m_baseSettings->setIsTypingVisible(visible);
    m_mouseSettings->setSwitchVisible(visible);
    m_touchSettingsGrp->setVisible(visible);
}

void MouseWidget::onTouchpadHideChanged(const bool visible)
{
    Q_UNUSED(visible);

    MouseModelMouseSettings *mouseModel = m_mouseModel->getMouseSettings();
    if (m_touchpadModel->getExist() && mouseModel->getExist()) {
        m_touchSettings->setVisible(!mouseModel->getSwitchState());
        return;
    }

    m_touchSettings->setVisible(m_touchpadModel->getExist());

}

void MouseWidget::onThinkapdVisibleChanged(const bool visible)
{
    m_ThinkapdSettingsGrp->setVisible(visible);
}
