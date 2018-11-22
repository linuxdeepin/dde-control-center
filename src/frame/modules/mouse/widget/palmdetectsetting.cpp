/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#include "palmdetectsetting.h"
#include "../mousemodel.h"
#include "widgets/settingsgroup.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/dccslider.h"

#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::mouse;
using namespace dcc::widgets;

PalmDetectSetting::PalmDetectSetting(QWidget *parent)
    : TranslucentFrame(parent)
    , m_detectSwitchBtn(new SwitchWidget(tr("Palm Detect"), this))
    , m_contactSlider(new TitledSliderItem(tr("Minimum contact surface"), this))
    , m_pressureSlider(new TitledSliderItem(tr("Minimum pressure value"), this))
{
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
    pressureSlider->setRange(0, 5);
    pressureSlider->setTickInterval(1);
    pressureSlider->setPageStep(1);

    QStringList pressureList;

    int i = 100;
    while (i <= 200) {
        pressureList << QString::number(i);
        i += 20;
    }

    m_pressureSlider->setAnnotations(pressureList);

    SettingsGroup *detectSwitchGrp = new SettingsGroup;
    detectSwitchGrp->appendItem(m_detectSwitchBtn);

    TipsLabel *tip = new TipsLabel(tr("Please disable the option if touchpad doesn't work after enabled"), this);
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
