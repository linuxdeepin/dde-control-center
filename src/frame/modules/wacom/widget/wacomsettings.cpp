/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "wacomsettings.h"
#include "widgets/settingsitem.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "../wacommodel.h"
#include "../model/wacommodelbase.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::wacom;

WacomSettings::WacomSettings(QWidget *parent):
    TranslucentFrame(parent)
{
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;
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
    setObjectName("WacomSettings");
    m_preSlider = m_pressureSlider->slider();
    connect(m_preSlider, &QSlider::valueChanged, this, &WacomSettings::requestSetPressureValue);
}

void WacomSettings::setModel(WacomModelBase *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &WacomModelBase::pressureValueChanged, this, &WacomSettings::setPressureValue);
    setPressureValue(m_baseSettings->getPressureValue());
}

void WacomSettings::setPressureValue(const int &value)
{
    m_preSlider->blockSignals(true);
    m_preSlider->setValue(value);
    m_preSlider->blockSignals(false);
}
