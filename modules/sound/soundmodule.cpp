/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundmodule.h"

SoundModule::SoundModule(FrameProxyInterface * frame, QObject *parent) :
    QObject(parent),
    ModuleInterface(frame),

    m_soundWidget(nullptr)
{

}

SoundModule::~SoundModule()
{
    m_soundWorker->deleteLater();
}

void SoundModule::initialize()
{
    m_soundWorker = new SoundWorker;

    m_soundWorker->moveToThread(qApp->thread());
}

const QString SoundModule::name() const
{
    return QStringLiteral("sound");
}

void SoundModule::moduleActive()
{
    m_soundWorker->activate();
}

void SoundModule::moduleDeactive()
{
    m_soundWorker->deactivate();
}

ModuleWidget *SoundModule::moduleWidget()
{
    if (!m_soundWidget) {
        m_soundWidget = new SoundWidget;
    }

    return m_soundWidget;
}

void SoundModule::contentPopped(ContentWidget * const)
{

}
