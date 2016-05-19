/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>

#include "sound.h"

#include "soundcontrol.h"
#include "soundview.h"

namespace Plugin
{
namespace Sound
{

SoundModule::SoundModule()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Sound"));

    qRegisterMetaType<Plugin::Sound::SoundModel>();

    m_control = new SoundControl();
    QThread *ctlThread = new QThread(this);
    connect(ctlThread, &QThread::started, m_control, &SoundControl::prepare);
    connect(ctlThread, &QThread::destroyed, m_control, &SoundControl::deleteLater);
    m_control->moveToThread(ctlThread);
    ctlThread->start();
}

QFrame *SoundModule::getContent()
{
    if (nullptr == m_view) {
        m_view = new SoundView(m_control);
    }
    return m_view;
}

SoundModule::~SoundModule()
{
    if (m_view) {
        m_view->deleteLater();
        m_view = nullptr;
    }
}


}
}
