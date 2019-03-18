/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "soundmodule.h"
#include "advancedpage.h"
#include "soundeffectpage.h"

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
        connect(m_soundWidget, &SoundWidget::requestSetSpeakerBalance, m_soundWorker, &SoundWorker::setSinkBalance);
        connect(m_soundWidget, &SoundWidget::requestSetMicrophoneVolume, m_soundWorker, &SoundWorker::setSourceVolume);
        connect(m_soundWidget, &SoundWidget::requestSetSpeakerVolume, m_soundWorker, &SoundWorker::setSinkVolume);
        connect(m_soundWidget, &SoundWidget::requestEffectPage, this, &SoundModule::showEffectPage);

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

void SoundModule::showEffectPage()
{
    m_soundWorker->refreshSoundEffect();

    SoundEffectPage *effectPage = new SoundEffectPage(m_soundModel);

    connect(effectPage, &SoundEffectPage::requestSetEffectEnable, m_soundWorker, &SoundWorker::setEffectEnable);
    connect(effectPage, &SoundEffectPage::requestEnableAllEffect, m_soundWorker, &SoundWorker::enableAllSoundEffect);

    m_frameProxy->pushWidget(this, effectPage);
}

}
}
