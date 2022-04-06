/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     duanhongyu <duanhongyu@uniontech.com>
*
* Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include <QApplication>
#include <DFontSizeManager>
#include "widgets/titlelabel.h"
#include "soundplugin.h"
#include "soundmodel.h"
#include "soundworker.h"
#include "speakerpage.h"
#include "microphonepage.h"
#include "soundeffectspage.h"
#include "devicemanagespage.h"

DWIDGET_USE_NAMESPACE

QString SoundPlugin::name() const
{
    return QStringLiteral("Sound");

}

ModuleObject *SoundPlugin::module()
{
    // 一级页面
    SoundModule *soundInterface = new SoundModule;
    soundInterface->setChildType(ModuleObject::ChildType::HList);

    // 二级 -- 输出
    ModuleObject *moduleOutput = new ModuleObject(tr("Output"), tr("Output"), this);
    moduleOutput->setChildType(ModuleObject::ChildType::Page);
    OutputModule *outputPage = new OutputModule(soundInterface->model(), soundInterface->work(), moduleOutput);
    moduleOutput->appendChild(outputPage);
    soundInterface->appendChild(moduleOutput);

    // 二级 -- 输入
    ModuleObject *moduleInput = new ModuleObject(tr("Input"), tr("Input"), this);
    moduleInput->setChildType(ModuleObject::ChildType::Page);
    InputModule *inputPage = new InputModule(soundInterface->model(), soundInterface->work(), moduleInput);
    moduleInput->appendChild(inputPage);
    soundInterface->appendChild(moduleInput);

    // 二级 -- 系统音效
    ModuleObject *moduleSoundEffects = new ModuleObject(tr("Sound Effects"), tr("Sound Effects"), this);
    moduleSoundEffects->setChildType(ModuleObject::ChildType::Page);
    SoundEffectsModule *effectsPage = new SoundEffectsModule(soundInterface->model(), soundInterface->work(), moduleSoundEffects);
    moduleSoundEffects->appendChild(effectsPage);
    soundInterface->appendChild(moduleSoundEffects);

    // 二级 -- 设备管理
    ModuleObject *moduleDevices = new ModuleObject(tr("Devices"), tr("Devices"), this);
    moduleDevices->setChildType(ModuleObject::ChildType::Page);

    DeviceTitleModule *inputTitle = new DeviceTitleModule(tr("Input Devices"), tr("Input Devices"), moduleDevices);
    moduleDevices->appendChild(inputTitle);
    InputDeviceModule *inputDevWidget = new  InputDeviceModule(soundInterface->model(), soundInterface->work(), moduleDevices);
    moduleDevices->appendChild(inputDevWidget);

    DeviceTitleModule *outputTitle = new DeviceTitleModule(tr("Output Devices"), tr("Output Devices"), moduleDevices);
    moduleDevices->appendChild(outputTitle);
    OutputDeviceModule *outputDevWidget = new  OutputDeviceModule(soundInterface->model(), soundInterface->work(), moduleDevices);
    moduleDevices->appendChild(outputDevWidget);

    soundInterface->appendChild(moduleDevices);
    return soundInterface;
}

SoundModule::SoundModule(QObject *parent)
    : ModuleObject("sound", tr("sound"), tr("sound"), QIcon::fromTheme("dcc_nav_sound"), parent)
    , m_model(new SoundModel(this))
    , m_work(new SoundWorker(m_model, this))
{
    m_model->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

SoundModule::~SoundModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void SoundModule::active()
{
    m_work->activate();
}

QWidget *OutputModule::page()
{
    SpeakerPage *w = new SpeakerPage;
    connect(w, &SpeakerPage::requestSetSpeakerBalance, m_worker, &SoundWorker::setSinkBalance);
    connect(w, &SpeakerPage::requestSetSpeakerVolume, m_worker, &SoundWorker::setSinkVolume);
    connect(w, &SpeakerPage::requestIncreaseVolume, m_worker, &SoundWorker::setIncreaseVolume);
    connect(w, &SpeakerPage::requestSetPort, m_worker, &SoundWorker::setPort);
    connect(w, &SpeakerPage::requestMute, m_worker, &SoundWorker::setSinkMute);
    connect(w, &SpeakerPage::requstBluetoothMode, m_worker, &SoundWorker::setBluetoothMode);
    w->setModel(m_model);
    return w;
}

QWidget *InputModule::page()
{
    MicrophonePage * w = new MicrophonePage;
    connect(w, &MicrophonePage::requestSetMicrophoneVolume, m_worker, &SoundWorker::setSourceVolume);
    connect(w, &MicrophonePage::requestSetPort, m_worker, &SoundWorker::setPort);
    connect(w, &MicrophonePage::requestReduceNoise, m_worker, &SoundWorker::setReduceNoise);
    connect(w, &MicrophonePage::requestMute, m_worker, &SoundWorker::setSourceMute);
    w->setModel(m_model);
    w->resetUi();
    return w;
}

QWidget *SoundEffectsModule::page()
{
    SoundEffectsPage *w = new SoundEffectsPage;
    connect(w, &SoundEffectsPage::requestSwitchSoundEffects, m_worker, &SoundWorker::enableAllSoundEffect);
    connect(w, &SoundEffectsPage::requestRefreshList, m_worker, &SoundWorker::refreshSoundEffect);
    connect(w, &SoundEffectsPage::requestSetEffectAble, m_worker, &SoundWorker::setEffectEnable);
    w->setModel(m_model);
    return w;
}

void SoundEffectsModule::active()
{
    m_worker->refreshSoundEffect();
}

QWidget *InputDeviceModule::page()
{
    DevicemanagesPage *w = new DevicemanagesPage(Port::Direction::In);
    connect(w, &DevicemanagesPage::requestSwitchSetEnable, m_worker, &SoundWorker::setPortEnabled);
    w->setModel(m_model);
    return w;
}

QWidget *OutputDeviceModule::page()
{
    DevicemanagesPage *w = new DevicemanagesPage(Port::Direction::Out);
    connect(w, &DevicemanagesPage::requestSwitchSetEnable, m_worker, &SoundWorker::setPortEnabled);
    w->setModel(m_model);
    return w;
}

DeviceTitleModule::DeviceTitleModule(const QString &name, const QString &title, QObject *parent)
{
    moduleData()->Name = name;
    moduleData()->Description = title;
    moduleData()->ContentText.append(title);
}

QWidget *DeviceTitleModule::page()
{
    TitleLabel *titleLabel = new TitleLabel(moduleData()->Description);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
    return titleLabel;
}
