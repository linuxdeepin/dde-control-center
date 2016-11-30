/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundwidget.h"
#include "dccslider.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc;

namespace dcc {
namespace sound {

SoundWidget::SoundWidget(SoundModel *model) :
    ModuleWidget(),
    m_speakerGroup(new SettingsGroup),
    m_speakerSwitch(new SwitchWidget),
    m_outputBalanceSliderItem(new TitledSliderItem(tr("Output Balance"))),
    m_microphoneGroup(new SettingsGroup),
    m_microphoneSwitch(new SwitchWidget),
    m_inputVolumeSliderItem(new TitledSliderItem(tr("Input Volume"))),
    m_inputFeedbackSliderItem(new TitledSliderItem(tr("Feedback Volume"))),
    m_soundEffectGroup(new SettingsGroup),
    m_soundEffectSwitch(new SwitchWidget)
{
    setTitle(tr("Sound"));

    m_speakerSwitch->setTitle(tr("Speaker"));

    m_outputBalanceSlider = m_outputBalanceSliderItem->slider();
    m_outputBalanceSlider->setType(DCCSlider::Vernier);
    m_outputBalanceSlider->setOrientation(Qt::Horizontal);
    m_outputBalanceSlider->setRange(-100, 100);
    m_outputBalanceSlider->setTickInterval(100);
    m_outputBalanceSlider->setTickPosition(QSlider::TicksBelow);

    m_speakerGroup->appendItem(m_speakerSwitch);
    m_speakerGroup->appendItem(m_outputBalanceSliderItem);

    m_microphoneSwitch->setTitle(tr("Microphone"));

    m_inputVolumeSlider = m_inputVolumeSliderItem->slider();
    m_inputVolumeSlider->setOrientation(Qt::Horizontal);
    m_inputVolumeSlider->setRange(0, 150);

    m_inputFeedbackSlider = m_inputFeedbackSliderItem->slider();
    m_inputFeedbackSlider->setType(DCCSlider::Progress);
    m_inputFeedbackSlider->setOrientation(Qt::Horizontal);
    m_inputFeedbackSlider->setRange(0, 100);

    m_microphoneGroup->appendItem(m_microphoneSwitch);
    m_microphoneGroup->appendItem(m_inputVolumeSliderItem);
    m_microphoneGroup->appendItem(m_inputFeedbackSliderItem);

    m_soundEffectSwitch->setTitle(tr("Sound Effect"));
    m_soundEffectGroup->appendItem(m_soundEffectSwitch);

    m_centeralLayout->addWidget(m_speakerGroup);
    m_centeralLayout->addWidget(m_microphoneGroup);
    m_centeralLayout->addWidget(m_soundEffectSwitch);

    setModel(model);

    connect(m_speakerSwitch, &SwitchWidget::checkedChanegd, this, &SoundWidget::requestSwitchSpeaker);
    connect(m_microphoneSwitch, &SwitchWidget::checkedChanegd, this, &SoundWidget::requestSiwtchMicrophone);
    connect(m_soundEffectSwitch, &SwitchWidget::checkedChanegd, this, &SoundWidget::requestSwitchSoundEffect);
    connect(m_outputBalanceSlider, &DCCSlider::valueChanged, [this] (double value) { emit requestSetSpeakerBalance(value / 100.f); });
    connect(m_inputVolumeSlider, &DCCSlider::valueChanged, [this] (double value) { emit requestSetMicrophoneVolume(value / 100.f); });
}

SoundWidget::~SoundWidget()
{

}

void SoundWidget::setModel(SoundModel *model)
{
    connect(model, &SoundModel::speakerOnChanged, [this] (bool on) {
        m_speakerSwitch->setChecked(on);
        m_outputBalanceSliderItem->setVisible(on);
    });
    connect(model, &SoundModel::microphoneOnChanged, [this] (bool on) {
        m_microphoneSwitch->setChecked(on);
        m_inputVolumeSliderItem->setVisible(on);
        m_inputFeedbackSliderItem->setVisible(on);
    });
    connect(model, &SoundModel::soundEffectOnChanged, m_soundEffectSwitch, &SwitchWidget::setChecked);
    connect(model, &SoundModel::speakerBalanceChanged, [this] (double value) {
        m_outputBalanceSlider->blockSignals(true);
        m_outputBalanceSlider->setValue(value * 100);
        m_outputBalanceSlider->blockSignals(false);
    });
    connect(model, &SoundModel::microphoneVolumeChanged, [this] (double value) {
        m_inputVolumeSlider->blockSignals(true);
        m_inputVolumeSlider->setValue(value * 100);
        m_inputVolumeSlider->blockSignals(false);
    });
    connect(model, &SoundModel::microphoneFeedbackChanged, [this] (double value) {
        m_inputFeedbackSlider->setValue(value * 100);
    });

    blockSignals(true);
    m_speakerSwitch->setChecked(model->speakerOn());
    m_microphoneSwitch->setChecked(model->microphoneOn());
    m_soundEffectSwitch->setChecked(model->soundEffectOn());
    m_outputBalanceSlider->setValue(model->speakerBalance());
    m_inputVolumeSlider->setValue(model->microphoneVolume());
    m_inputFeedbackSlider->setValue(model->microphoneFeedback());
    blockSignals(false);
}

}
}
