/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundwidget.h"

#include <QVBoxLayout>
#include <QSlider>

SoundWidget::SoundWidget() :
    ModuleWidget(),
    m_speakerGroup(new SettingsGroup),
    m_speakerSwitch(new SwitchWidget),
    m_outputVolumeSliderItem(new TitledSettingsItem),
    m_outputBalanceSliderItem(new TitledSettingsItem),
    m_microphoneGroup(new SettingsGroup),
    m_microphoneSwitch(new SwitchWidget),
    m_inputVolumeSliderItem(new TitledSettingsItem),
    m_inputFeedbackSliderItem(new TitledSettingsItem),
    m_soundEffectGroup(new SettingsGroup),
    m_soundEffectSwitch(new SwitchWidget)
{
    setTitle(tr("Sound"));

    m_speakerSwitch->setTitle(tr("Speaker"));
    m_outputVolumeSliderItem->setTitle(tr("Output Volume"));
    QSlider * outputVolumeSlider = new QSlider;
    outputVolumeSlider->setOrientation(Qt::Horizontal);
    m_outputVolumeSliderItem->setWidget(outputVolumeSlider);
    m_outputBalanceSliderItem->setTitle(tr("Output Balance"));
    QSlider * outputBalanceSlider = new QSlider;
    outputBalanceSlider->setOrientation(Qt::Horizontal);
    m_outputBalanceSliderItem->setWidget(outputBalanceSlider);
    m_speakerGroup->appendItem(m_speakerSwitch);
    m_speakerGroup->appendItem(m_outputVolumeSliderItem);
    m_speakerGroup->appendItem(m_outputBalanceSliderItem);

    m_microphoneSwitch->setTitle(tr("Microphone"));
    m_inputVolumeSliderItem->setTitle(tr("Input Volume"));
    QSlider * inputVolumeSlider = new QSlider;
    inputVolumeSlider->setOrientation(Qt::Horizontal);
    m_inputVolumeSliderItem->setWidget(inputVolumeSlider);
    m_inputFeedbackSliderItem->setTitle(tr("Feedback Volume"));
    QSlider * inputFeedbackSlider = new QSlider;
    inputFeedbackSlider->setOrientation(Qt::Horizontal);
    m_inputFeedbackSliderItem->setWidget(inputFeedbackSlider);
    m_microphoneGroup->appendItem(m_microphoneSwitch);
    m_microphoneGroup->appendItem(m_inputVolumeSliderItem);
    m_microphoneGroup->appendItem(m_inputFeedbackSliderItem);

    m_soundEffectSwitch->setTitle(tr("Sound Effect"));
    m_soundEffectGroup->appendItem(m_soundEffectSwitch);

    m_centeralLayout->addWidget(m_speakerGroup);
    m_centeralLayout->addWidget(m_microphoneGroup);
    m_centeralLayout->addWidget(m_soundEffectSwitch);
}

SoundWidget::~SoundWidget()
{

}
