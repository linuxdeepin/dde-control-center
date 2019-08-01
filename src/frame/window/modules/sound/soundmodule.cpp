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

#include "soundmodule.h"
#include "soundwidget.h"
#include "speakerpage.h"
#include "microphonepage.h"
#include "advancedpage.h"
#include "soundeffectspage.h"

#include "modules/sound/soundmodel.h"
#include "modules/sound/soundworker.h"
#include "modules/sound/soundmodel.h"

using namespace dcc::sound;
using namespace DCC_NAMESPACE::sound;

SoundModule::SoundModule(FrameProxyInterface *frameProxy, QObject *parent):
    QObject(parent),
    ModuleInterface(frameProxy)
{

}

void SoundModule::initialize()
{
    m_model = new SoundModel;
    m_worker = new SoundWorker(m_model);

    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

void SoundModule::reset()
{
}

void SoundModule::contentPopped(QWidget *const w)
{
    w->deleteLater();
}

const QString SoundModule::name() const
{
    return tr("Sound");
}

QWidget *SoundModule::moduleWidget()
{
    SoundWidget*  widget = new SoundWidget();

    connect(widget, &SoundWidget::requsetSpeakerPage, this, &SoundModule::showSpeakerPage);
    connect(widget, &SoundWidget::requestMicrophonePage, this, &SoundModule::showMicrophonePage);
    connect(widget, &SoundWidget::requestAdvancedPage, this, &SoundModule::showAdvancedPage);
    connect(widget, &SoundWidget::requsetSoundEffectsPage, this, &SoundModule::showSoundEffectsPage);

    return widget;
}

void SoundModule::showSpeakerPage()
{
    SpeakerPage *w = new SpeakerPage;

    w->setModel(m_model);
    connect(w, &SpeakerPage::requestSwitchSpeaker, m_worker, &SoundWorker::switchSpeaker);
    connect(w, &SpeakerPage::requestSetSpeakerBalance, m_worker, &SoundWorker::setSinkBalance);
    connect(w, &SpeakerPage::requestSetSpeakerVolume, m_worker, &SoundWorker::setSinkVolume);

    m_frameProxy->pushWidget(this, w);
}

void SoundModule::showMicrophonePage()
{
    MicrophonePage *w = new MicrophonePage;
    connect(w, &MicrophonePage::requestSwitchMicrophone, m_worker, &SoundWorker::switchMicrophone);
    connect(w, &MicrophonePage::requestSetMicrophoneVolume, m_worker, &SoundWorker::setSourceVolume);

    w->setModel(m_model);
    m_frameProxy->pushWidget(this, w);
}

void SoundModule::showAdvancedPage()
{
    AdvancedPage *w = new AdvancedPage;

    m_frameProxy->pushWidget(this, w);
}

void SoundModule::showSoundEffectsPage()
{
    SoundEffectsPage *w = new SoundEffectsPage;

    m_frameProxy->pushWidget(this, w);
}
