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

#include "thinkpadsettings.h"
#include "settingsitem.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include "titledslideritem.h"
#include "settingshead.h"
#include "mouse/model/mousemodelthinkpadsettings.h"
#include "dccslider.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::mouse;

ThinkpadSettings::ThinkpadSettings(QWidget *parent)
    : TranslucentFrame(parent)
{
    m_mainGroup = new SettingsGroup(tr("TrackPoint"));
    m_mainLayout = new QVBoxLayout;
    m_title     = new SettingsHead;
    m_title->setTitle(tr("Thinkpad Track Point"));
    m_title->setEditEnable(false);

    m_speedSlider = new widgets::TitledSliderItem(tr("Pointer Speed"));
    m_speedSlider->slider()->setType(DCCSlider::Vernier);
    m_speedSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_speedSlider->slider()->setRange(0, 6);
    m_speedSlider->slider()->setTickInterval(1);
    m_speedSlider->slider()->setPageStep(1);

    QStringList list;
    list << tr("Slow") << "" << "" << "" << "" << "" << tr("Fast");
    m_speedSlider->setAnnotations(list);

    m_mainGroup->appendItem(m_speedSlider);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);

    QSlider *spSlider;
    spSlider = m_speedSlider->slider();
    connect(spSlider, &QSlider::valueChanged, this, &ThinkpadSettings::requestSetSliderValue);
    setObjectName("ThinkpadSettings");
}

void ThinkpadSettings::setModel(MouseModelThinkpadSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelThinkpadSettings::sliderValueChanged, this, &ThinkpadSettings::setSliderValue);
    connect(m_baseSettings, &MouseModelThinkpadSettings::existChanged, this, &ThinkpadSettings::setVisible);
    setSliderValue(m_baseSettings->getSliderValue());
    setVisible(m_baseSettings->getExist());
}

void ThinkpadSettings::setSliderValue(const int &value)
{
    QSlider *spSlider = m_speedSlider->slider();
    spSlider->blockSignals(true);
    spSlider->setValue(value);
    spSlider->blockSignals(false);
}
