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

#include "microphonepage.h"
#include "modules/sound/soundmodel.h"
#include "window/utils.h"

#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

using namespace dcc::sound;
using namespace dcc::widgets;

#include "modules/sound/soundmodel.h"

#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <DStyle>
#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

using namespace dcc::sound;
using namespace dcc::widgets;

using namespace DCC_NAMESPACE::sound;

const double BrightnessMaxScale = 100.0;
const int PercentageNum = 100;

MicrophonePage::MicrophonePage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_sw(new SwitchWidget)
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    TitleLabel *lblTitle = new TitleLabel(tr("Microphone"));
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_sw = new SwitchWidget(nullptr, lblTitle);
    hlayout->addWidget(m_sw);
    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->addLayout(hlayout);
    setLayout(m_layout);
}


MicrophonePage::~MicrophonePage()
{
#ifndef DCC_DISABLE_FEEDBACK
    if (m_feedbackSlider)
        m_feedbackSlider->disconnect(m_conn);
    m_feedbackSlider->deleteLater();
#endif
}

void MicrophonePage::setModel(SoundModel *model)
{
    m_model = model;

    m_sw->setChecked(m_model->microphoneOn());
    connect(m_sw, &SwitchWidget::checkedChanged, this, &MicrophonePage::requestSwitchMicrophone);
    connect(m_sw, &SwitchWidget::checkedChanged, this, [ = ](bool checked) {
        if (checked) {
            //enable slider
        } else {
            //disable slider
        }
    });
    connect(m_model, &SoundModel::microphoneOnChanged, m_sw, &SwitchWidget::setChecked);

    initSlider();
}

void MicrophonePage::initSlider()
{
    //~ contents_path /sound/Microphone
    m_inputSlider = new TitledSliderItem(tr("Input Volume"));
    m_inputSlider->addBackground();
    m_inputSlider->setVisible(m_model->microphoneOn());
    m_layout->insertWidget(1, m_inputSlider);

    DCCSlider *slider = m_inputSlider->slider();
    slider->setRange(0, PercentageNum);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::NoTicks);
    auto icon_low = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeLowElement);
    slider->setLeftIcon(icon_low);
    auto icon_high = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeHighElement);
    slider->setRightIcon(icon_high);
    slider->setIconSize(QSize(24, 24));
    slider->setTickInterval(1);
    slider->setSliderPosition(qRound(m_model->microphoneVolume() * PercentageNum));
    slider->setPageStep(1);

    auto slotfunc1 = [ = ](int pos) {
        double val = pos / BrightnessMaxScale;
        Q_EMIT requestSetMicrophoneVolume(val);
    };
    int val = qRound(m_model->microphoneVolume() * BrightnessMaxScale);
    slider->setValue(val);
    m_inputSlider->setValueLiteral(QString::number(m_model->microphoneVolume() * PercentageNum) + "%");
    connect(slider, &DCCSlider::valueChanged, this, slotfunc1);
    connect(slider, &DCCSlider::sliderMoved, this, slotfunc1);
    connect(m_model, &SoundModel::microphoneOnChanged, m_inputSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::microphoneVolumeChanged, this, [ = ](double v) {
        slider->blockSignals(true);
        slider->setValue(qRound(v * PercentageNum));
        slider->setSliderPosition(qRound(v * PercentageNum));
        slider->blockSignals(false);
        m_inputSlider->setValueLiteral(QString::number(qRound(v * PercentageNum)) + "%");
    });
#ifndef DCC_DISABLE_FEEDBACK
    //~ contents_path /sound/Microphone
    m_feedbackSlider = (new TitledSliderItem(tr("Input Level")));
    m_feedbackSlider->addBackground();
    m_feedbackSlider->setVisible(m_model->microphoneOn());
    DCCSlider *slider2 = m_feedbackSlider->slider();
    slider2->setRange(0, PercentageNum);
    slider2->setEnabled(false);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::NoTicks);
    slider2->setLeftIcon(QIcon::fromTheme("dcc_feedbacklow"));
    slider2->setRightIcon(QIcon::fromTheme("dcc_feedbackhigh"));
    slider2->setIconSize(QSize(24, 24));
    slider2->setTickInterval(1);
    slider2->setPageStep(1);

    connect(m_model, &SoundModel::microphoneOnChanged, m_feedbackSlider, &TitledSliderItem::setVisible);
    m_conn = connect(m_model, &SoundModel::microphoneFeedbackChanged, [ = ](double vol2) {
        qDebug() << "sound input ,start feedback changed" << vol2;
        //这里有可能导致精度丢失，所以采用了qRound 4舍5入取正
        slider2->setSliderPosition(qRound(vol2 * PercentageNum));
    });
    m_layout->setSpacing(10);
    m_layout->insertWidget(2, m_feedbackSlider);
    m_layout->addStretch(10);
#endif
}
