/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "speakerpage.h"

#include "modules/sound/soundmodel.h"

#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;

SpeakerPage::SpeakerPage(QWidget *parent)
    : QWidget(parent)
    , m_sw(new SwitchWidget)
    , m_outputSlider(new TitledSliderItem(tr("Output Volume")))
    , m_balanceSlider(new TitledSliderItem(tr("Left/Right Balance")))
    , m_layout(new QVBoxLayout)
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(new QLabel(tr("Speaker")));
    hlayout->addStretch(1);
    hlayout->addWidget(m_sw);

    m_layout->addLayout(hlayout);
    m_layout->addStretch(1);
    setLayout(m_layout);
}

SpeakerPage::~SpeakerPage()
{
}

void SpeakerPage::setModel(dcc::sound::SoundModel *model)
{
    m_model = model;

    m_sw->setChecked(m_model->speakerOn());
    connect(m_sw, &SwitchWidget::checkedChanged, this, &SpeakerPage::requestSwitchSpeaker);
    connect(m_sw, &SwitchWidget::checkedChanged, this, [ = ](bool checked) {
        if (checked) {
            //enable slider
        } else {
            //disable slider
        }
    });
    connect(m_model, &SoundModel::speakerOnChanged, m_sw, &SwitchWidget::setChecked);

    initSlider();
}

void SpeakerPage::initSlider()
{
    auto outputSlider = new TitledSliderItem(tr("Output Volume"), this);
    DCCSlider *slider = outputSlider->slider();

    slider->setRange(0, 100);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setSliderPosition(static_cast<int>(m_model->speakerVolume() * 100.0));
    slider->setPageStep(1);

    auto slotfunc1 = [ = ](int pos) {
        double val = pos / 100.0;
        Q_EMIT requestSetSpeakerVolume(val);
    };
    connect(slider, &DCCSlider::valueChanged, slotfunc1);
    connect(slider, &DCCSlider::sliderMoved, slotfunc1);
    m_layout->insertWidget(1, outputSlider);

    auto balanceSlider = new TitledSliderItem(tr("Left/Right Balance"), this);
    DCCSlider *slider2 = balanceSlider->slider();
    slider2->setRange(-100, 100);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::TicksBelow);
    slider2->setTickInterval(1);
    slider2->setSliderPosition(static_cast<int>(m_model->speakerBalance() * 100));
    slider2->setPageStep(1);

    auto slotfunc2 = [ = ](int pos) {
        double val = pos / 100.0;
        Q_EMIT requestSetSpeakerBalance(val);
    };
    connect(slider2, &DCCSlider::valueChanged, slotfunc2);
    connect(slider2, &DCCSlider::sliderMoved, slotfunc2);

    m_layout->insertWidget(2, balanceSlider);
}
