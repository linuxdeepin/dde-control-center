// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "touchpadsettingwidget.h"
#include "window/utils.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/contentwidget.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/mouseworker.h"

#include <QDebug>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc;
using namespace dcc::mouse;
using namespace dcc::widgets;

TouchPadSettingWidget::TouchPadSettingWidget(QWidget *parent)
    : QWidget(parent)
{
    //~ contents_path /mouse/Touchpad
    //~ child_page Touchpad
    m_touchPadEnable = new SwitchWidget(tr("Touchpad"));
    m_touchPadEnable->setObjectName("touchPadEnabled");

    //~ contents_path /mouse/Touchpad
    //~ child_page Touchpad
    m_touchMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    //~ contents_path /mouse/Touchpad
    //~ child_page Touchpad
    m_touchClickStn = new SwitchWidget(tr("Tap to Click"));
    m_touchClickStn->setObjectName("touchClicked");
    m_touchClickStn->addBackground();
    //~ contents_path /mouse/Touchpad
    //~ child_page Touchpad
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
    m_contentLayout->setSpacing(List_Interval);
    m_contentLayout->setContentsMargins(ThirdPageCmbMargins);
    m_contentLayout->addWidget(m_touchMoveSlider);
    m_contentLayout->addWidget(m_touchClickStn);
    m_contentLayout->addWidget(m_touchNaturalScroll);
    m_contentLayout->addWidget(m_palmDetectSetting);
    m_contentLayout->addStretch();

    QWidget *widget = new QWidget(this);
    widget->setAccessibleName("TouchpadSettingWidget_widget");
    widget->setContentsMargins(2, 2, 2, 5);
    widget->setLayout(m_contentLayout);
    m_contentWidget = new ContentWidget(this);
    m_contentWidget->setAccessibleName("ShortCutSettingWidget_ContentWidget");
    m_contentWidget->setContent(widget);
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    vlayout->addWidget(m_touchPadEnable);
    vlayout->addWidget(m_contentWidget);
    setLayout(vlayout);

    connect(m_touchPadEnable, &SwitchWidget::checkedChanged, [this] (const bool state) {
        m_contentWidget->setVisible(state);
        Q_EMIT requestSetTouchpadEnable(state);
    });
    connect(m_touchMoveSlider->slider(), &DCCSlider::valueChanged, [this](int value) {
        requestSetTouchpadMotionAcceleration(value);
    });
    connect(m_touchClickStn, &SwitchWidget::checkedChanged, this, &TouchPadSettingWidget::requestSetTapClick);
    connect(m_touchNaturalScroll, &SwitchWidget::checkedChanged, this, &TouchPadSettingWidget::requestSetTouchNaturalScroll);
}

void TouchPadSettingWidget::setModel(dcc::mouse::MouseModel *const model)
{
    m_mouseModel = model;
    connect(model, &MouseModel::tpadMoveSpeedChanged, this, [this] (int value) {
        onTouchMoveSpeedChanged(value);
    });
    connect(model, &MouseModel::tapClickChanged, m_touchClickStn, &SwitchWidget::setChecked);
    connect(model, &MouseModel::touchpadEnableChanged, m_touchPadEnable, &SwitchWidget::setChecked);
    connect(model, &MouseModel::touchpadEnableChanged, m_contentWidget, &ContentWidget::setVisible);
    connect(model, &MouseModel::tpadNaturalScrollChanged, m_touchNaturalScroll, &SwitchWidget::setChecked);

    m_palmDetectSetting->setModel(model);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestContact, this, &TouchPadSettingWidget::requestContact);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestDetectState, this, &TouchPadSettingWidget::requestDetectState);
    connect(m_palmDetectSetting, &PalmDetectSetting::requestPressure, this, &TouchPadSettingWidget::requestPressure);

    m_touchPadEnable->setVisible(m_mouseModel->systemTouchpadExist());
    m_touchPadEnable->setChecked(m_mouseModel->touchpadEnable());
    m_contentWidget->setVisible(m_mouseModel->touchpadEnable() || !m_mouseModel->systemTouchpadExist());
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
