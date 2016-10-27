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

SoundWidget::SoundWidget() :
    ModuleWidget(),
    m_speakerGroup(new SettingsGroup),
    m_speakerSwitch(new SwitchWidget),
    m_microphoneGroup(new SettingsGroup),
    m_microphoneSwitch(new SwitchWidget)
{
    setTitle(tr("Sound"));

    m_speakerGroup->appendItem(m_speakerSwitch);
    m_microphoneGroup->appendItem(m_microphoneSwitch);

    m_centeralLayout->addWidget(m_speakerGroup);
    m_centeralLayout->addWidget(m_microphoneGroup);
}

SoundWidget::~SoundWidget()
{

}
