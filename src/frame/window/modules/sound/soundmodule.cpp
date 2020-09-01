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
#include "soundeffectspage.h"

#include "modules/sound/soundmodel.h"
#include "modules/sound/soundworker.h"
#include "modules/sound/soundmodel.h"

using namespace dcc::sound;
using namespace DCC_NAMESPACE::sound;

SoundModule::SoundModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
{

}

void SoundModule::initialize()
{
    if (m_model) {
        delete m_model;
    }
    m_model = new SoundModel;
    m_worker = new SoundWorker(m_model);

    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

const QString SoundModule::name() const
{
    return "sound";
}

const QString SoundModule::displayName() const
{
    return tr("Sound");
}

void SoundModule::active()
{
    m_worker->activate();
    m_soundWidget = new SoundWidget();

    connect(m_soundWidget, &SoundWidget::requsetSpeakerPage, this, &SoundModule::showSpeakerPage);
    connect(m_soundWidget, &SoundWidget::requestMicrophonePage, this, &SoundModule::showMicrophonePage);
    connect(m_soundWidget, &SoundWidget::requsetSoundEffectsPage, this, &SoundModule::showSoundEffectsPage);

    m_frameProxy->pushWidget(this, m_soundWidget);
    showSpeakerPage();
}

int SoundModule::load(const QString &path)
{
    if (!m_soundWidget)
        active();

    return m_soundWidget->showPath(path);
}

QStringList SoundModule::availPage() const
{
    QStringList sl;
    sl << "Speaker" << "Microphone" << "Advanced" << "Sound Effects";

    return sl;
}

void SoundModule::showSpeakerPage()
{
    SpeakerPage *w = new SpeakerPage;

    m_model->setPortEnable(false);
    w->setModel(m_model);
    connect(w, &SpeakerPage::requestSetSpeakerBalance, m_worker, &SoundWorker::setSinkBalance);
    connect(w, &SpeakerPage::requestSetSpeakerVolume, m_worker, &SoundWorker::setSinkVolume);
    connect(w, &SpeakerPage::requestIncreaseVolume, m_worker, &SoundWorker::setIncreaseVolume);
    connect(w, &SpeakerPage::requestSetPort, m_worker, &SoundWorker::setPort);
    m_frameProxy->pushWidget(this, w);
}

void SoundModule::showMicrophonePage()
{
    MicrophonePage *w = new MicrophonePage;
    m_model->setPortEnable(false);
    w->setModel(m_model);
    m_model->initMicroPhone();
    connect(w, &MicrophonePage::requestSetMicrophoneVolume, m_worker, &SoundWorker::setSourceVolume);
    connect(w, &MicrophonePage::requestSetPort, m_worker, &SoundWorker::setPort);
    connect(w, &MicrophonePage::requestReduceNoise, m_worker, &SoundWorker::setReduceNoise);
    connect(w, &MicrophonePage::requestMute, m_worker, &SoundWorker::setMute);
    m_frameProxy->pushWidget(this, w);
    //输出端口重置后可能会出现，默认输入为空，重置界面
    w->resetUi();
}

void SoundModule::showSoundEffectsPage()
{
    m_worker->refreshSoundEffect();
    SoundEffectsPage *w = new SoundEffectsPage;

    connect(w, &SoundEffectsPage::requestRefreshList, m_worker, &SoundWorker::refreshSoundEffect);
    connect(w, &SoundEffectsPage::requestSwitchSoundEffects, m_worker, &SoundWorker::enableAllSoundEffect);
    connect(w, &SoundEffectsPage::requestSetEffectAble, m_worker, &SoundWorker::setEffectEnable);
    w->setModel(m_model);
    m_frameProxy->pushWidget(this, w);
}
