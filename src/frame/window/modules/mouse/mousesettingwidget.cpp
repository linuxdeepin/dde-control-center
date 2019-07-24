/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
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
#include "mousesettingwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "../../../modules/mouse/widget/palmdetectsetting.h"
#include "widgets/dccslider.h"
#include "../../../modules/mouse/widget/doutestwidget.h"
#include "../../../modules/mouse/mousemodel.h"
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace dcc::mouse;
using namespace dcc::widgets;

MouseSettingWidget::MouseSettingWidget(QWidget *parent) : ContentWidget(parent)
{
    m_mouseSettingsGrp = new SettingsGroup;

    m_mouseMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_adaptiveAccelProfile = new SwitchWidget(tr("Mouse Acceleration"));
    m_disTchStn = new SwitchWidget(tr("Disable the touchpad when inserting the mouse"));
    m_mouseNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));

    m_mouseSettingsGrp->appendItem(m_mouseMoveSlider);
    m_mouseSettingsGrp->appendItem(m_adaptiveAccelProfile);
    m_mouseSettingsGrp->appendItem(m_disTchStn);
    m_mouseSettingsGrp->appendItem(m_mouseNaturalScroll);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->addWidget(m_mouseSettingsGrp);
    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(m_contentLayout);
    setContent(w);
    connect(m_mouseMoveSlider->slider(), &DCCSlider::valueChanged, this, &MouseSettingWidget::requestSetMouseMotionAcceleration);
    connect(m_adaptiveAccelProfile, &SwitchWidget::checkedChanged, this, &MouseSettingWidget::requestSetAccelProfile);
    connect(m_disTchStn, &SwitchWidget::checkedChanged, this, &MouseSettingWidget::requestSetDisTouchPad);
    connect(m_mouseNaturalScroll, &SwitchWidget::checkedChanged, this, &MouseSettingWidget::requestSetMouseNaturalScroll);
}

void MouseSettingWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;

    connect(model, &MouseModel::mouseMoveSpeedChanged, this, &MouseSettingWidget::onMouseMoveSpeedChanged);
    connect(model, &MouseModel::accelProfileChanged, m_adaptiveAccelProfile, &SwitchWidget::setChecked);
    connect(model, &MouseModel::disTpadChanged, m_disTchStn, &SwitchWidget::setChecked);
    connect(model, &MouseModel::mouseNaturalScrollChanged, m_mouseNaturalScroll, &SwitchWidget::setChecked);

    onMouseMoveSpeedChanged(model->mouseMoveSpeed());
    m_adaptiveAccelProfile->setChecked(model->accelProfile());
    m_disTchStn->setChecked(model->disTpad());
    m_mouseNaturalScroll->setChecked(model->mouseNaturalScroll());
}

void MouseSettingWidget::onMouseMoveSpeedChanged(int speed)
{
    m_mouseMoveSlider->slider()->blockSignals(true);
    m_mouseMoveSlider->slider()->setValue(speed);
    m_mouseMoveSlider->slider()->blockSignals(false);
}
