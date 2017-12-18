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
#include "mousemodel.h"
#include "widget/palmdetectsetting.h"
#include "dccslider.h"
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

    setTitle(tr("Mouse and Touchpad"));

    m_baseSettingsGrp = new SettingsGroup;
    m_mouseSettingsGrp = new SettingsGroup(tr("Mouse"));
    m_touchSettingsGrp = new SettingsGroup(tr("TouchPad"));
    m_thinkapdSettingsGrp = new SettingsGroup(tr("TrackPoint"));

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

    QStringList doublelist;
    doublelist << tr("Slow") << "" << "" << "" << "" << "" << tr("Fast");

    QList<TitledSliderItem*> tmpList;
    tmpList <<  m_doubleSlider << m_mouseMoveSlider << m_touchMoveSlider << m_trackMoveSlider;

    for (TitledSliderItem* item : tmpList) {
        DCCSlider *slider = item->slider();
        slider->setType(DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setRange(0, 6);
        slider->setTickInterval(1);
        slider->setPageStep(1);
        item->setAnnotations(doublelist);
    }

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

    m_thinkapdSettingsGrp->appendItem(m_trackMoveSlider);

    m_centralLayout->addWidget(m_baseSettingsGrp);
    m_centralLayout->addWidget(m_mouseSettingsGrp);
    m_centralLayout->addWidget(m_touchSettingsGrp);
    m_centralLayout->addWidget(m_thinkapdSettingsGrp);

    connect(m_leftHand, &SwitchWidget::checkedChanged, this, &MouseWidget::requestSetLeftHand);
    connect(m_disInTyping, &SwitchWidget::checkedChanged, this, &MouseWidget::requestSetDisTyping);
    connect(m_disTchStn, &SwitchWidget::checkedChanged, this, &MouseWidget::requestSetDisTouchPad);

    connect(m_touchClickStn, &SwitchWidget::checkedChanged, this, &MouseWidget::requestSetTapClick);

    connect(m_mouseNaturalScroll, &SwitchWidget::checkedChanged, this, &MouseWidget::requestSetMouseNaturalScroll);
    connect(m_touchNaturalScroll, &SwitchWidget::checkedChanged, this, &MouseWidget::requestSetTouchNaturalScroll);

    connect(m_doubleSlider->slider(), &DCCSlider::valueChanged, this, &MouseWidget::requestSetDouClick);
    connect(m_mouseMoveSlider->slider(), &DCCSlider::valueChanged, this, &MouseWidget::requestSetMouseMotionAcceleration);
    connect(m_touchMoveSlider->slider(), &DCCSlider::valueChanged, this, &MouseWidget::requestSetTouchpadMotionAcceleration);
    connect(m_trackMoveSlider->slider(), &DCCSlider::valueChanged, this, &MouseWidget::requestSetTrackPointMotionAcceleration);
}

void MouseWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;

    connect(model, &MouseModel::mouseExistChanged, m_mouseSettingsGrp, &SettingsGroup::setVisible);
    connect(model, &MouseModel::tpadExistChanged, this, &MouseWidget::onTouchpadHideChanged);
    connect(model, &MouseModel::redPointExistChanged, m_thinkapdSettingsGrp, &SettingsGroup::setVisible);

    connect(model, &MouseModel::doubleSpeedChanged, this, &MouseWidget::onDoubleClickSpeedChanged);
    connect(model, &MouseModel::mouseMoveSpeedChanged, this, &MouseWidget::onMouseMoveSpeedChanged);
    connect(model, &MouseModel::tpadMoveSpeedChanged, this, &MouseWidget::onTouchMoveSpeedChanged);
    connect(model, &MouseModel::redPointMoveSpeedChanged, this, &MouseWidget::onRedPointMoveSpeedChanged);

    connect(model, &MouseModel::mouseNaturalScrollChanged, m_mouseNaturalScroll, &SwitchWidget::setChecked);
    connect(model, &MouseModel::tpadNaturalScrollChanged, m_touchNaturalScroll, &SwitchWidget::setChecked);

    connect(model, &MouseModel::disTpadChanged, m_disTchStn, &SwitchWidget::setChecked);
    connect(model, &MouseModel::disTpadChanged, this, &MouseWidget::onTouchpadHideChanged);

    connect(model, &MouseModel::disIfTypingStateChanged, m_disInTyping, &SwitchWidget::setChecked);
    connect(model, &MouseModel::tapClickChanged, m_touchClickStn, &SwitchWidget::setChecked);

    m_mouseSettingsGrp->setVisible(model->mouseExist());
    m_thinkapdSettingsGrp->setVisible(model->redPointExist());

    onMouseMoveSpeedChanged(model->mouseMoveSpeed());
    onTouchMoveSpeedChanged(model->tpadMoveSpeed());
    onTouchpadHideChanged(model->tpadExist());

    onRedPointMoveSpeedChanged(model->redPointMoveSpeed());
    onDoubleClickSpeedChanged(model->doubleSpeed());

    m_mouseNaturalScroll->setChecked(model->mouseNaturalScroll());
    m_touchNaturalScroll->setChecked(model->tpadNaturalScroll());
    m_disTchStn->setChecked(model->disTpad());
    m_touchClickStn->setChecked(model->getTapclick());
}

void MouseWidget::onTouchpadHideChanged(const bool visible)
{
    Q_UNUSED(visible);

    if (m_mouseModel->tpadExist() && m_mouseModel->mouseExist()) {
        m_touchSettingsGrp->setVisible(!m_mouseModel->disTpad());
        return;
    }

    m_touchSettingsGrp->setVisible(m_mouseModel->disTpad());
}

void MouseWidget::onMouseMoveSpeedChanged(int speed)
{
    m_mouseMoveSlider->slider()->blockSignals(true);
    m_mouseMoveSlider->slider()->setValue(speed);
    m_mouseMoveSlider->slider()->blockSignals(false);
}

void MouseWidget::onTouchMoveSpeedChanged(int speed)
{
    m_touchMoveSlider->slider()->blockSignals(true);
    m_touchMoveSlider->slider()->setValue(speed);
    m_touchMoveSlider->slider()->blockSignals(false);
}

void MouseWidget::onRedPointMoveSpeedChanged(int speed)
{
    m_trackMoveSlider->slider()->blockSignals(true);
    m_trackMoveSlider->slider()->setValue(speed);
    m_trackMoveSlider->slider()->blockSignals(false);
}

void MouseWidget::onDoubleClickSpeedChanged(int speed)
{
    m_doubleSlider->slider()->blockSignals(true);
    m_doubleSlider->slider()->setValue(speed);
    m_doubleSlider->slider()->blockSignals(false);
}
