/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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
    //~ contents_path /wacon/Pressure Sensitivity
    //~ child_page Pressure Sensitivity
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
