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
#include "touchpadsettingwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/mouseworker.h"
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;
using namespace dcc::widgets;

TouchPadSettingWidget::TouchPadSettingWidget(QWidget *parent) : dcc::ContentWidget(parent)
{
    m_touchpadSettingsGrp = new SettingsGroup;

    m_touchMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_touchClickStn = new SwitchWidget(tr("Tap to Click"));
    m_touchNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));
    m_palmDetectSetting = new PalmDetectSetting;

    QStringList touchMoveList;
    touchMoveList << tr("Slow") << "" << "" << "" << "" << "" << tr("Fast");

    DCCSlider *touchSlider = m_touchMoveSlider->slider();
    touchSlider->setType(DCCSlider::Vernier);
    touchSlider->setTickPosition(QSlider::TicksBelow);
    touchSlider->setRange(0, 6);
    touchSlider->setTickInterval(1);
    touchSlider->setPageStep(1);
    m_touchMoveSlider->setAnnotations(touchMoveList);

    m_touchpadSettingsGrp->appendItem(m_touchMoveSlider);
    m_touchpadSettingsGrp->appendItem(m_touchClickStn);
    m_touchpadSettingsGrp->appendItem(m_touchNaturalScroll);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->addWidget(m_touchpadSettingsGrp);
    m_contentLayout->addWidget(m_palmDetectSetting);
    m_contentLayout->addStretch();
    TranslucentFrame *tFrame = new TranslucentFrame;
    tFrame->setLayout(m_contentLayout);
    setContent(tFrame);

    connect(m_touchMoveSlider->slider(), &DCCSlider::valueChanged, this, &TouchPadSettingWidget::requestSetTouchpadMotionAcceleration);
    connect(m_touchClickStn, &SwitchWidget::checkedChanged, this, &TouchPadSettingWidget::requestSetTapClick);
    connect(m_touchNaturalScroll, &SwitchWidget::checkedChanged, this, &TouchPadSettingWidget::requestSetTouchNaturalScroll);
}

void TouchPadSettingWidget::setModel(dcc::mouse::MouseModel *const model)
{
    m_mouseModel = model;
    connect(model, &MouseModel::tpadMoveSpeedChanged, this, &TouchPadSettingWidget::onTouchMoveSpeedChanged);
    connect(model, &MouseModel::tapClickChanged, m_touchClickStn, &SwitchWidget::setChecked);
    connect(model, &MouseModel::tpadNaturalScrollChanged, m_touchNaturalScroll, &SwitchWidget::setChecked);

    m_palmDetectSetting->setModel(model);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestContact, this, &TouchPadSettingWidget::requestContact);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestDetectState, this, &TouchPadSettingWidget::requestDetectState);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestPressure, this, &TouchPadSettingWidget::requestPressure);

    onTouchMoveSpeedChanged(m_mouseModel->tpadMoveSpeed());
    m_touchClickStn->setChecked(m_mouseModel->tapclick());
    m_touchNaturalScroll->setChecked(m_mouseModel->tpadNaturalScroll());
}

void TouchPadSettingWidget::onTouchMoveSpeedChanged(int speed)
{
    m_touchMoveSlider->slider()->blockSignals(true);
    m_touchMoveSlider->slider()->setValue(speed);
    m_touchMoveSlider->slider()->blockSignals(false);
}
