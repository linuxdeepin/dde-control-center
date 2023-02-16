//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "touchpadsettingwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "palmdetectsetting.h"
#include "doutestwidget.h"
#include "src/plugin-mouse/operation/mousemodel.h"

#include <QDebug>
#include <QVBoxLayout>

const QMargins ThirdPageContentsMargins(0, 10, 0, 10);

using namespace DCC_NAMESPACE;
TouchPadSettingWidget::TouchPadSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    m_touchMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_touchClickStn = new SwitchWidget(tr("Tap to Click"));
    m_touchClickStn->setObjectName("touchClicked");
    m_touchClickStn->addBackground();
    m_touchNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));
    m_touchNaturalScroll->setObjectName("touchNaturalScroll");
    m_touchNaturalScroll->addBackground();

    m_palmDetectSetting = new PalmDetectSetting;
    m_palmDetectSetting->setVisible(false);

    QStringList touchMoveList;
    touchMoveList << tr("Slow") << "" << "" << "" << "" << "";
    touchMoveList << tr("Fast");

    DCCSlider *touchSlider = m_touchMoveSlider->slider();
    touchSlider->setType(DCCSlider::Vernier);
    touchSlider->setTickPosition(QSlider::TicksBelow);
    touchSlider->setRange(0, 6);
    touchSlider->setTickInterval(1);
    touchSlider->setPageStep(1);
    m_touchMoveSlider->setAnnotations(touchMoveList);
    m_touchMoveSlider->addBackground();

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setSpacing(10);
    m_contentLayout->setContentsMargins(ThirdPageContentsMargins);
    m_contentLayout->addWidget(m_touchMoveSlider);
    m_contentLayout->addWidget(m_touchClickStn);
    m_contentLayout->addWidget(m_touchNaturalScroll);
    m_contentLayout->addWidget(m_palmDetectSetting);
    m_contentLayout->addStretch();
    setLayout(m_contentLayout);

    connect(m_touchMoveSlider->slider(), &DCCSlider::valueChanged, [this](int value) {
        requestSetTouchpadMotionAcceleration(value);
    });
    connect(m_touchClickStn, &SwitchWidget::checkedChanged, this, &TouchPadSettingWidget::requestSetTapClick);
    connect(m_touchNaturalScroll, &SwitchWidget::checkedChanged, this, &TouchPadSettingWidget::requestSetTouchNaturalScroll);
}

void TouchPadSettingWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;
    connect(model, &MouseModel::tpadMoveSpeedChanged, this, [this] (int value) {
        onTouchMoveSpeedChanged(value);
    });
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
