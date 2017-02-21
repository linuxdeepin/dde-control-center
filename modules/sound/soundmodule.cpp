/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundmodule.h"
#include "advancedpage.h"

namespace dcc {
namespace sound {

SoundModule::SoundModule(FrameProxyInterface * frame, QObject *parent) :
    QObject(parent),
    ModuleInterface(frame),

    m_soundWidget(nullptr)
{

}

SoundModule::~SoundModule()
{
    m_soundWorker->deleteLater();
    m_soundModel->deleteLater();
}

void SoundModule::initialize()
{
    m_soundModel = new SoundModel;
    m_soundWorker = new SoundWorker(m_soundModel);

    m_soundModel->moveToThread(qApp->thread());
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

void SoundModule::reset()
{

}

ModuleWidget *SoundModule::moduleWidget()
{
    if (!m_soundWidget) {
        m_soundWidget = new SoundWidget(m_soundModel);

        connect(m_soundWidget, &SoundWidget::requestSwitchSpeaker, m_soundWorker, &SoundWorker::switchSpeaker);
        connect(m_soundWidget, &SoundWidget::requestSiwtchMicrophone, m_soundWorker, &SoundWorker::switchMicrophone);
        connect(m_soundWidget, &SoundWidget::requestSwitchSoundEffect, m_soundWorker, &SoundWorker::switchSoundEffect);
        connect(m_soundWidget, &SoundWidget::requestSetSpeakerBalance, m_soundWorker, &SoundWorker::setSinkBalance);
        connect(m_soundWidget, &SoundWidget::requestSetMicrophoneVolume, m_soundWorker, &SoundWorker::setSourceVolume);
        connect(m_soundWidget, &SoundWidget::requestSetSpeakerVolume, m_soundWorker, &SoundWorker::setSinkVolume);

        connect(m_soundWidget, &SoundWidget::requestAdvancedPage, this, &SoundModule::showAdvancedPage);
    }

    return m_soundWidget;
}

void SoundModule::contentPopped(ContentWidget * const)
{

}

void SoundModule::showAdvancedPage()
{
    AdvancedPage *page = new AdvancedPage(m_soundModel);

    connect(page, &AdvancedPage::requestSetPort, m_soundWorker, &SoundWorker::setPort);

    m_frameProxy->pushWidget(this, page);
}

}
}
