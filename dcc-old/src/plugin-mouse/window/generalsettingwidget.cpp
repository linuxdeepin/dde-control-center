// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "generalsettingwidget.h"

#include "doutestwidget.h"
#include "palmdetectsetting.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"

#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;

GeneralSettingWidget::GeneralSettingWidget(QWidget *parent)
    : QWidget(parent)
    , m_mouseModel(nullptr)
    , m_generalSettingsGrp(nullptr)
{
    QFrame *frame = new QFrame(this);
    frame->setAccessibleName("GeneralSettingWidget_frame");
    m_generalSettingsGrp = new SettingsGroup(frame);

    m_leftHand = new SwitchWidget(tr("Left Hand"), frame);
    m_leftHand->setObjectName("leftHand");

    m_disInTyping = new SwitchWidget(tr("Disable touchpad while typing"), frame);
    m_disInTyping->setObjectName("disInTyping");

    m_scrollSpeedSlider = new TitledSliderItem(tr("Scrolling Speed"), frame);
    m_scrollSpeedSlider->setObjectName("scrollSpeed");

    m_doubleSlider = new TitledSliderItem(tr("Double-click Speed"), frame);
    m_doubleSlider->setObjectName("doubleClicked");

    m_doubleTest = new DouTestWidget(this);

    DCCSlider *speedSlider = m_scrollSpeedSlider->slider();
    speedSlider->setType(DCCSlider::Vernier);
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setRange(1, 10);
    speedSlider->setTickInterval(1);
    speedSlider->setPageStep(1);
    QStringList speedList;
    speedList << tr("Slow") << ""
              << ""
              << ""
              << ""
              << ""
              << ""
              << ""
              << "" << tr("Fast");
    m_scrollSpeedSlider->setAnnotations(speedList);

    QStringList doublelist;
    doublelist << tr("Slow") << ""
               << ""
               << ""
               << ""
               << "";
    doublelist << tr("Fast");
    DCCSlider *doubleSlider = m_doubleSlider->slider();
    doubleSlider->setType(DCCSlider::Vernier);
    doubleSlider->setTickPosition(QSlider::TicksBelow);
    doubleSlider->setRange(0, 6);
    doubleSlider->setTickInterval(1);
    doubleSlider->setPageStep(1);
    m_doubleSlider->setAnnotations(doublelist);

    m_generalSettingsGrp->setSpacing(10);
    m_generalSettingsGrp->appendItem(m_leftHand);
    m_generalSettingsGrp->appendItem(m_disInTyping);
    m_generalSettingsGrp->appendItem(m_scrollSpeedSlider);
    m_generalSettingsGrp->appendItem(m_doubleSlider);
    m_generalSettingsGrp->appendItem(m_doubleTest);

    m_contentLayout = new QVBoxLayout(this);
    m_contentLayout->addWidget(m_generalSettingsGrp);
    m_contentLayout->setAlignment(Qt::AlignTop);
    m_contentLayout->setSpacing(10);
    m_contentLayout->setContentsMargins(0, 10, 0, 5); // 右侧间距为10 补下面的 8
    setLayout(m_contentLayout);

    connect(m_leftHand,
            &SwitchWidget::checkedChanged,
            this,
            &GeneralSettingWidget::requestSetLeftHand);
    connect(m_disInTyping,
            &SwitchWidget::checkedChanged,
            this,
            &GeneralSettingWidget::requestSetDisTyping);
    connect(m_scrollSpeedSlider->slider(),
            &DCCSlider::valueChanged,
            this,
            &GeneralSettingWidget::requestScrollSpeed);
    connect(m_doubleSlider->slider(),
            &DCCSlider::valueChanged,
            this,
            &GeneralSettingWidget::requestSetDouClick);
}

GeneralSettingWidget::~GeneralSettingWidget() { }

void GeneralSettingWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;

    connect(model, &MouseModel::tpadExistChanged, m_disInTyping, &SwitchWidget::setVisible);
    connect(model, &MouseModel::leftHandStateChanged, m_leftHand, &SwitchWidget::setChecked);
    connect(model, &MouseModel::disIfTypingStateChanged, m_disInTyping, &SwitchWidget::setChecked);
    connect(model,
            &MouseModel::doubleSpeedChanged,
            this,
            &GeneralSettingWidget::onDoubleClickSpeedChanged);
    connect(model,
            &MouseModel::scrollSpeedChanged,
            this,
            &GeneralSettingWidget::onScrollSpeedChanged);
    m_leftHand->setChecked(model->leftHandState());
    m_disInTyping->setChecked(model->disIfTyping());
    m_disInTyping->setVisible(model->tpadExist());
    onDoubleClickSpeedChanged(model->doubleSpeed());
    onScrollSpeedChanged(model->scrollSpeed());
}

void GeneralSettingWidget::onDoubleClickSpeedChanged(int speed)
{
    speed = qBound(0, speed, 6);
    m_doubleSlider->slider()->blockSignals(true);
    m_doubleSlider->slider()->setValue(speed);
    m_doubleSlider->slider()->blockSignals(false);
}

void GeneralSettingWidget::onScrollSpeedChanged(uint speed)
{
    m_scrollSpeedSlider->slider()->blockSignals(true);
    m_scrollSpeedSlider->slider()->setValue(static_cast<int>(speed));
    m_scrollSpeedSlider->slider()->blockSignals(false);
}
