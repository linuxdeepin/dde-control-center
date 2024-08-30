//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "palmdetectsetting.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "widgets/settingsgroup.h"
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;

PalmDetectSetting::PalmDetectSetting(QWidget *parent)
    : QFrame(parent)
    , m_detectSwitchBtn(new SwitchWidget(tr("Palm Detection"), this))
    , m_contactSlider(new TitledSliderItem(tr("Minimum Contact Surface"), this))
    , m_pressureSlider(new TitledSliderItem(tr("Minimum Pressure Value"), this))
{
    m_detectSwitchBtn->setTitle(tr("Palm Detection"));
    m_contactSlider->setToolTip(tr("Minimum Contact Surface"));
    m_contactSlider->addBackground();
    m_contactSlider->setObjectName("contact");
    m_pressureSlider->setToolTip(tr("Minimum Pressure Value"));
    m_pressureSlider->addBackground();
    m_pressureSlider->setObjectName("pressure");

    QStringList contactList;
    for (int i(1); i <= 10; ++i) {
        contactList << QString::number(i);
    }

    // NOTE(kirigaya): The contact is only between 1 and 10.
    DCCSlider *contactSlider = m_contactSlider->slider();
    contactSlider->setType(DCCSlider::Vernier);
    contactSlider->setTickPosition(QSlider::TicksBelow);
    contactSlider->setRange(1, 10);
    contactSlider->setTickInterval(1);
    contactSlider->setPageStep(1);

    m_contactSlider->setAnnotations(contactList);

    // NOTE(kirigaya): The range of pressure is between 100 and 200
    DCCSlider *pressureSlider = m_pressureSlider->slider();
    pressureSlider->setType(DCCSlider::Vernier);
    pressureSlider->setTickPosition(QSlider::TicksBelow);
    pressureSlider->setRange(0, 10);
    pressureSlider->setTickInterval(1);
    pressureSlider->setPageStep(1);

    QStringList pressureList;

    int i = 100;
    while (i <= 200) {
        pressureList << QString::number(i);
        i += 10;
    }

    m_pressureSlider->setAnnotations(pressureList);

    SettingsGroup *detectSwitchGrp = new SettingsGroup;
    detectSwitchGrp->appendItem(m_detectSwitchBtn);

    QLabel *tip = new QLabel(tr("Disable the option if touchpad doesn't work after enabled"), this);
    tip->setWordWrap(true);
    tip->setContentsMargins(16, 5, 10, 5);

    SettingsGroup *sliderGrp = new SettingsGroup;

    sliderGrp->appendItem(m_contactSlider);
    sliderGrp->appendItem(m_pressureSlider);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(detectSwitchGrp);
    layout->addWidget(tip);
    layout->addWidget(sliderGrp);

    setLayout(layout);

    connect(m_detectSwitchBtn, &SwitchWidget::checkedChanged, this, &PalmDetectSetting::requestDetectState);
    connect(contactSlider, &DCCSlider::valueChanged, this, &PalmDetectSetting::requestContact);
    connect(pressureSlider, &DCCSlider::valueChanged, this, [=](int value) {
        // the valid value is 100-200 and step should be 20
        requestPressure(value * 20 + 100);
    });
}

void PalmDetectSetting::setModel(MouseModel * const model)
{
    connect(model, &MouseModel::palmDetectChanged, this, &PalmDetectSetting::setDetectState);
    connect(model, &MouseModel::palmMinWidthChanged, this, &PalmDetectSetting::setContactValue);
    connect(model, &MouseModel::palmMinzChanged, this, &PalmDetectSetting::setPressureValue);

    setDetectState(model->palmDetect());
    setContactValue(model->palmMinWidth());
    setPressureValue(model->palmMinz());
}

void PalmDetectSetting::setDetectState(bool enable)
{
    m_detectSwitchBtn->setChecked(enable);
    m_contactSlider->setVisible(enable);
    m_pressureSlider->setVisible(enable);
}

void PalmDetectSetting::setContactValue(int value)
{
    DCCSlider *contactSlider = m_contactSlider->slider();
    contactSlider->blockSignals(true);
    contactSlider->setValue(value);
    contactSlider->blockSignals(false);
}

void PalmDetectSetting::setPressureValue(int value)
{
    DCCSlider *pressureSlider = m_pressureSlider->slider();
    pressureSlider->blockSignals(true);
    pressureSlider->setValue((value - 100) / 20);
    pressureSlider->blockSignals(false);
}
