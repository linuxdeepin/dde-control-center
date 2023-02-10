/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *             Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
 *             Tianlu Shao <shaotianlu@uniontech.com>
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

#include "doutestwidget.h"
#include "palmdetectsetting.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;

MouseSettingWidget::MouseSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    m_mouseSettingsGrp = new SettingsGroup;
    m_mouseMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_adaptiveAccelProfile = new SwitchWidget(tr("Mouse Acceleration"));
    m_adaptiveAccelProfile->setAccessibleName(tr("Mouse Acceleration"));
    m_adaptiveAccelProfile->setObjectName("adaptiveAccelProfile");
    m_disTchStn = new SwitchWidget(tr("Disable touchpad when a mouse is connected"));
    m_disTchStn->setAccessibleName(tr("Disable touchpad when a mouse is connected"));
    m_disTchStn->setObjectName("disableTouchPad");
    m_mouseNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));
    m_mouseNaturalScroll->setAccessibleName(tr("Natural Scrolling"));
    m_mouseNaturalScroll->setObjectName("mouseNaturalScroll");

    QStringList speedList;
    speedList << tr("Slow") << ""
              << ""
              << ""
              << ""
              << "";
    speedList << tr("Fast");
    DCCSlider *speedSlider = m_mouseMoveSlider->slider();
    speedSlider->setType(DCCSlider::Vernier);
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setRange(0, 6);
    speedSlider->setTickInterval(1);
    speedSlider->setPageStep(1);
    m_mouseMoveSlider->setAnnotations(speedList);

    m_mouseSettingsGrp->setSpacing(10);
    m_mouseSettingsGrp->appendItem(m_mouseMoveSlider);
    m_mouseSettingsGrp->appendItem(m_adaptiveAccelProfile);
    m_mouseSettingsGrp->appendItem(m_disTchStn);
    m_mouseSettingsGrp->appendItem(m_mouseNaturalScroll);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->addWidget(m_mouseSettingsGrp);
    m_contentLayout->setAlignment(Qt::AlignTop);
    m_contentLayout->setSpacing(10);
    m_contentLayout->setContentsMargins(0, 10, 0, 5);

    setLayout(m_contentLayout);

    connect(m_mouseMoveSlider->slider(),
            &DCCSlider::valueChanged,
            this,
            &MouseSettingWidget::requestSetMouseMotionAcceleration);
    connect(m_adaptiveAccelProfile,
            &SwitchWidget::checkedChanged,
            this,
            &MouseSettingWidget::requestSetAccelProfile);
    connect(m_disTchStn,
            &SwitchWidget::checkedChanged,
            this,
            &MouseSettingWidget::requestSetDisTouchPad);
    connect(m_mouseNaturalScroll,
            &SwitchWidget::checkedChanged,
            this,
            &MouseSettingWidget::requestSetMouseNaturalScroll);
}

MouseSettingWidget::~MouseSettingWidget() { }

void MouseSettingWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;

    connect(model, &MouseModel::tpadExistChanged, m_disTchStn, &SwitchWidget::setVisible);
    connect(model,
            &MouseModel::mouseMoveSpeedChanged,
            this,
            &MouseSettingWidget::onMouseMoveSpeedChanged);
    connect(model,
            &MouseModel::accelProfileChanged,
            m_adaptiveAccelProfile,
            &SwitchWidget::setChecked);
    connect(model, &MouseModel::disTpadChanged, m_disTchStn, &SwitchWidget::setChecked);
    connect(model,
            &MouseModel::mouseNaturalScrollChanged,
            m_mouseNaturalScroll,
            &SwitchWidget::setChecked);

    onMouseMoveSpeedChanged(model->mouseMoveSpeed());
    m_adaptiveAccelProfile->setChecked(model->accelProfile());
    m_disTchStn->setChecked(model->disTpad());
    m_disTchStn->setVisible(model->tpadExist());
    m_mouseNaturalScroll->setChecked(model->mouseNaturalScroll());
}

void MouseSettingWidget::onMouseMoveSpeedChanged(int speed)
{
    m_mouseMoveSlider->slider()->blockSignals(true);
    m_mouseMoveSlider->slider()->setValue(speed);
    m_mouseMoveSlider->slider()->blockSignals(false);
}
