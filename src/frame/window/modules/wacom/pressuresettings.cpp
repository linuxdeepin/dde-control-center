// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pressuresettings.h"
#include "window/modules/wacom/wacommodel.h"

#include "widgets/settingsitem.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"

using namespace dcc;
using namespace dcc::widgets;

namespace DCC_NAMESPACE {
namespace wacom {

PressureSettings::PressureSettings(QWidget *parent)
    : TranslucentFrame(parent)
    , m_model(nullptr)
{
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
    //~ contents_path /wacom/Pressure Sensitivity
    m_pressureSlider = new TitledSliderItem(tr("Pressure Sensitivity"));
    m_pressureSlider->slider()->setType(DCCSlider::Vernier);
    m_pressureSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_pressureSlider->slider()->setRange(1, 7);
    m_pressureSlider->slider()->setTickInterval(1);
    m_pressureSlider->slider()->setPageStep(1);

    QStringList delays;
    delays<<tr("Light")<<""<<""<<""<<""<<""<<tr("Heavy");
    m_pressureSlider->setAnnotations(delays);
    m_mainGroup->appendItem(m_pressureSlider);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);

    m_preSlider = qobject_cast<DSlider *>(m_pressureSlider->slider());
    connect(m_preSlider, &DSlider::valueChanged, this, &PressureSettings::requestSetPressureValue);
}

void PressureSettings::setModel(WacomModel *model)
{
    m_model = model;

    connect(m_model, &WacomModel::pressureValueChanged, this, &PressureSettings::setPressureValue);
    setPressureValue(m_model->getPressureValue());
}

void PressureSettings::setPressureValue(const uint &value)
{
    m_preSlider->blockSignals(true);
    m_preSlider->setValue(int(value));
    m_preSlider->blockSignals(false);
}

}
}
