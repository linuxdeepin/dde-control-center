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
#include "window/utils.h"
#include "widgets/titlelabel.h"

#include <DStyle>

#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;
DWIDGET_USE_NAMESPACE

SpeakerPage::SpeakerPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
{
    m_sw = new SwitchWidget(nullptr, new TitleLabel(tr("Speaker")));

    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->setSpacing(25);
    m_layout->addWidget(m_sw);
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
    connect(m_model, &SoundModel::speakerOnChanged, m_sw, &SwitchWidget::setChecked);

    initSlider();
}

void SpeakerPage::initSlider()
{
    //~ contents_path /sound/Speaker
    auto outputSlider = new TitledSliderItem(tr("Output Volume"), this);
    outputSlider->addBackground();
    outputSlider->setVisible(m_model->speakerOn());
    DCCSlider *slider = outputSlider->slider();

    slider->setRange(0, 150);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::NoTicks);
    auto icon_low = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeLowElement);
    outputSlider->setLeftIcon(icon_low);
    auto icon_high = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeHighElement);
    outputSlider->setRightIcon(icon_high);
    outputSlider->setIconSize(QSize(24, 24));
    slider->setTickInterval(1);
    slider->setValue(static_cast<int>(m_model->speakerVolume() * 100.0));
    slider->setPageStep(1);

    auto slotfunc1 = [ = ](int pos) {
        double val = pos / 100.0;
        Q_EMIT requestSetSpeakerVolume(val);
    };
    outputSlider->setValueLiteral(QString::number(int(m_model->speakerVolume() * 100)) + "%");
    connect(slider, &DCCSlider::valueChanged, slotfunc1);
    connect(slider, &DCCSlider::sliderMoved, slotfunc1);
    connect(m_model, &SoundModel::speakerOnChanged, outputSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::speakerVolumeChanged, this, [ = ](double v) {
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(v * 100));
        slider->blockSignals(false);
        outputSlider->setValueLiteral(QString::number(int(v * 100)) + "%");
    });

    m_layout->insertWidget(1, outputSlider);

    //~ contents_path /sound/Speaker
    auto balanceSlider = new TitledSliderItem(tr("Left/Right Balance"), this);
    balanceSlider->addBackground();
    balanceSlider->setVisible(m_model->speakerOn());

    QStringList balanceList;
    balanceList << tr("Left")<< " ";
    balanceList << tr("Right");
    DCCSlider *slider2 = balanceSlider->slider();
    slider2->setRange(-100, 100);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::TicksBelow);
    slider2->setTickInterval(1);
    slider2->setSliderPosition(static_cast<int>(m_model->speakerBalance() * 100));
    slider2->setPageStep(1);
    balanceSlider->setAnnotations(balanceList);

    auto slotfunc2 = [ = ](int pos) {
        double val = pos / 100.0;
        Q_EMIT requestSetSpeakerBalance(val);
    };
    connect(slider2, &DCCSlider::valueChanged, slotfunc2);
    connect(slider2, &DCCSlider::sliderMoved, slotfunc2);
    connect(m_model, &SoundModel::speakerOnChanged, balanceSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::speakerBalanceChanged, this, [ = ](double v) {
        slider2->blockSignals(true);
        slider2->setSliderPosition(static_cast<int>(v * 100));
        slider2->blockSignals(false);
    });

    m_layout->insertWidget(2, balanceSlider);
}
