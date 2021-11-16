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
#include "microphonepage.h"
#include "modules/sound/soundmodel.h"
#include "modules/sound/soundworker.h"
#include "devicemanagespage.h"
#include "soundeffectspage.h"
#include "soundwidget.h"
#include "speakerpage.h"
#include "window/mainwindow.h"
#include "window/gsettingwatcher.h"

using namespace dcc::sound;
using namespace DCC_NAMESPACE::sound;

SoundModule::SoundModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_outputPortCount(0)
    , m_inputPortCount(0)
{
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
    GSettingWatcher::instance()->insertState("soundInput");
    GSettingWatcher::instance()->insertState("soundOutput");
    GSettingWatcher::instance()->insertState("soundEffects");
    GSettingWatcher::instance()->insertState("deviceManage");
}

void SoundModule::preInitialize(bool, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    if (m_model) {
        delete m_model;
    }
    m_model = new SoundModel;
    m_worker = new SoundWorker(m_model);

    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());

    initSearchData();

    connect(m_model, &SoundModel::balanceVisibleChanged, m_pMainWindow, &MainWindow::setSpecialThreeMenuVisible);
}

void SoundModule::initSearchData()
{
    QString module = tr("Sound");
    QString devices = tr("Devices");
    QString input = tr("Input");
    QString soundEffects = tr("Sound Effects");
    QString output = tr("Output");

    QString leftRightBalance = tr("Left/Right Balance").remove('/').trimmed();

    QStringList gsSeclist;
    gsSeclist << "soundOutput" << "soundInput" << "soundEffects" << "deviceManage";
    QStringList gsThirdlist;
    gsThirdlist << "soundInputSlider" << "soundOutputSlider" << "soundVolumeBoost"
                << "soundBalanceSlider" << "soundEffectPage" << "soundNoiseReduce";

    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [ = ](const QString &gsettings, QString state = "") {
        if (gsettings == "") {
            return false;
        }

        bool ret = false;
        if (state == "") {
            ret = GSettingWatcher::instance()->get(gsettings).toBool();
        } else {
            ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        }

        gsettingsMap.insert(gsettings, ret);

        return ret;
    };

    auto func_input_changed = [ = ]() {
        bool bSoundInput = func_is_visible("soundInput");
        m_frameProxy->setWidgetVisible(module, input, bSoundInput);
        m_frameProxy->setDetailVisible(module, input, tr("Input Device"), bSoundInput);
        //输入设备为空不显示
        bool isInputVisble = m_inputPortCount > 0;
        m_frameProxy->setDetailVisible(module, input, tr("Automatic Noise Suppression"), bSoundInput && func_is_visible("soundNoiseReduce", "Hidden") && isInputVisble);
        m_frameProxy->setDetailVisible(module, input, tr("Input Volume"), bSoundInput && func_is_visible("soundInputSlider", "Hidden") && isInputVisble);
        m_frameProxy->setDetailVisible(module, input, tr("Input Level"), bSoundInput && isInputVisble);
    };

    auto func_output_changed = [ = ]() {
        bool bSoundOutput = func_is_visible("soundOutput");
        m_frameProxy->setWidgetVisible(module, output, bSoundOutput);
        m_frameProxy->setDetailVisible(module, output, tr("Output Device"), bSoundOutput);
        //输出设备为空不显示
        bool isOutputVisble = m_outputPortCount > 0;
        m_frameProxy->setDetailVisible(module, output, tr("Output Volume"), bSoundOutput && func_is_visible("soundOutputSlider", "Hidden") && isOutputVisble);
        m_frameProxy->setDetailVisible(module, output, tr("Volume Boost"),  bSoundOutput && func_is_visible("soundVolumeBoost", "Hidden") && isOutputVisble);
        m_frameProxy->setDetailVisible(module, output, leftRightBalance, bSoundOutput && func_is_visible("soundBalanceSlider", "Hidden") && isOutputVisble);
    };

    auto func_device_changed = [ = ]() {
        bool bDevice = func_is_visible("deviceManage");
        m_frameProxy->setWidgetVisible(module, devices, bDevice);
        //输出设备不为空
        m_frameProxy->setDetailVisible(module, devices, tr("Output Devices"), bDevice && m_outputPortCount > 0);
        //输入设备不为空
        m_frameProxy->setDetailVisible(module, devices, tr("Input Devices"), bDevice && m_inputPortCount > 0);
    };

    auto func_effect_changed = [ = ]() {
        bool bSoundEffects = func_is_visible("soundEffects");
        m_frameProxy->setWidgetVisible(module, soundEffects, bSoundEffects);
        m_frameProxy->setDetailVisible(module, soundEffects, soundEffects, bSoundEffects && func_is_visible("soundEffectPage", "Hidden"));
    };

    auto func_process_all = [ = ]() {
        getPortCount();

        m_frameProxy->setModuleVisible(module, true);

        func_output_changed();

        func_input_changed();

        func_effect_changed();

        func_device_changed();
    };

    //TODO: 当有输入或输出设备时可能不显示，该问题需要解决
    connect(m_model, &SoundModel::portAdded, this, [ = ](const Port *port) {
        func_process_all();
        m_frameProxy->updateSearchData(module);
    });

    connect(m_model, &SoundModel::portRemoved, this, [ = ](const QString portName, const uint &cardId) {
        func_process_all();
        m_frameProxy->updateSearchData(module);
    });

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
            return;
        }

        bool bGsMap = gsettingsMap.value(gsetting);
        if (gsSeclist.contains(gsetting) && GSettingWatcher::instance()->get(gsetting).toBool() == bGsMap) {
            return;
        }

        if (gsThirdlist.contains(gsetting)  && ((state != "Hidden") == bGsMap)) {
            return;
        }

        if ("soundOutput" == gsetting) {
            func_output_changed();
        } else if ("soundInput" == gsetting) {
            func_input_changed();
        } else if ("soundEffects" == gsetting) {
            func_effect_changed();
        } else if ("deviceManage" == gsetting) {
            func_device_changed();
        } else if ("soundInputSlider" == gsetting) {
            //输入设备为空不显示
            m_frameProxy->setDetailVisible(module, input, tr("Input Volume"), func_is_visible("soundInput") && func_is_visible("soundInputSlider", "Hidden") && m_inputPortCount > 0);
        } else if ("soundOutputSlider" == gsetting) {
            //输出设备为空不显示
            m_frameProxy->setDetailVisible(module, output, tr("Output Volume"), func_is_visible("soundOutput") && func_is_visible("soundOutputSlider", "Hidden") && m_outputPortCount > 0);
        } else if ("soundVolumeBoost" == gsetting) {
            //输出设备为空不显示
            m_frameProxy->setDetailVisible(module, output, tr("Volume Boost"),  func_is_visible("soundOutput") && func_is_visible("soundVolumeBoost", "Hidden") && m_outputPortCount > 0);
        } else if ("soundBalanceSlider" == gsetting) {
            //输出设备为空不显示
            m_frameProxy->setDetailVisible(module, output, leftRightBalance, func_is_visible("soundOutput") && func_is_visible("soundBalanceSlider", "Hidden") && m_outputPortCount > 0);
        } else if ("soundEffectPage" == gsetting) {
            bool bSoundEffects = func_is_visible("soundEffects");
            m_frameProxy->setDetailVisible(module, soundEffects, soundEffects, bSoundEffects && func_is_visible("soundEffectPage", "Hidden"));
        } else if ("soundNoiseReduce" == gsetting) {
            //输入设备为空不显示
            m_frameProxy->setDetailVisible(module, input, tr("Automatic Noise Suppression"), func_is_visible("soundInput") && func_is_visible("soundNoiseReduce", "Hidden") && m_inputPortCount > 0);
        } else {
            qInfo() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void SoundModule::getPortCount()
{
    m_inputPortCount = 0;
    m_outputPortCount = 0;
    QList<Port *> ports = m_model->ports();
    for (int i = 0; i < ports.size(); i++) {
        Port * port = ports.value(i);
        if (port->direction() == Port::Out) {
            m_outputPortCount ++;
        } else {
            m_inputPortCount ++;
        }
    }
}

void SoundModule::initialize()
{
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
    m_soundWidget->setVisible(false);
    connect(m_soundWidget, &SoundWidget::requsetSpeakerPage, this, &SoundModule::showSpeakerPage);
    connect(m_soundWidget, &SoundWidget::requestMicrophonePage, this, &SoundModule::showMicrophonePage);
    connect(m_soundWidget, &SoundWidget::requsetSoundEffectsPage, this, &SoundModule::showSoundEffectsPage);
    connect(m_soundWidget, &SoundWidget::requsetDeviceManagesPage, this, &SoundModule::showDeviceManagesPage);
    connect(m_soundWidget, &SoundWidget::requestUpdateSecondMenu, this, [=](bool needPop) {
        if (m_pMainWindow->getcontentStack().size() >= 2 && needPop) {
            m_frameProxy->popWidget(this);
        }
        m_soundWidget->showDefaultWidget();
    });

    m_frameProxy->pushWidget(this, m_soundWidget);
    m_soundWidget->setVisible(true);
    m_soundWidget->showDefaultWidget();
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
    sl << "Speaker" << "Microphone" << "Sound Effects" << "Devices";

    return sl;
}

void SoundModule::showSpeakerPage()
{
    SpeakerPage *w = new SpeakerPage;

    m_model->setPortEnable(false);
    w->setVisible(false);
    connect(w, &SpeakerPage::requestSetSpeakerBalance, m_worker, &SoundWorker::setSinkBalance);
    connect(w, &SpeakerPage::requestSetSpeakerVolume, m_worker, &SoundWorker::setSinkVolume);
    connect(w, &SpeakerPage::requestIncreaseVolume, m_worker, &SoundWorker::setIncreaseVolume);
    connect(w, &SpeakerPage::requestSetPort, m_worker, &SoundWorker::setPort);
    connect(w, &SpeakerPage::requestMute, m_worker, &SoundWorker::setSinkMute);
    connect(w, &SpeakerPage::requstBluetoothMode, m_worker, &SoundWorker::setBluetoothMode);
    w->setModel(m_model);
    m_model->initSpeaker();
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
}

void SoundModule::showMicrophonePage()
{
    MicrophonePage *w = new MicrophonePage;
    m_model->setPortEnable(false);
    w->setVisible(false);
    w->setModel(m_model);
    m_model->initMicroPhone();
    connect(w, &MicrophonePage::requestSetMicrophoneVolume, m_worker, &SoundWorker::setSourceVolume);
    connect(w, &MicrophonePage::requestSetPort, m_worker, &SoundWorker::setPort);
    connect(w, &MicrophonePage::requestReduceNoise, m_worker, &SoundWorker::setReduceNoise);
    connect(w, &MicrophonePage::requestMute, m_worker, &SoundWorker::setSourceMute);
    m_frameProxy->pushWidget(this, w);
    //输出端口重置后可能会出现，默认输入为空，重置界面
    w->resetUi();
    w->setVisible(true);
}

void SoundModule::showSoundEffectsPage()
{
    m_worker->refreshSoundEffect();
    SoundEffectsPage *w = new SoundEffectsPage;
    w->setVisible(false);
    connect(w, &SoundEffectsPage::requestRefreshList, m_worker, &SoundWorker::refreshSoundEffect);
    connect(w, &SoundEffectsPage::requestSwitchSoundEffects, m_worker, &SoundWorker::enableAllSoundEffect);
    connect(w, &SoundEffectsPage::requestSetEffectAble, m_worker, &SoundWorker::setEffectEnable);
    w->setModel(m_model);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
}

void SoundModule::showDeviceManagesPage()
{
    DevicemanagesPage *w = new DevicemanagesPage;
    w->setVisible(false);
    connect(w, &DevicemanagesPage::requestSwitchSetEnable, m_worker, &SoundWorker::setPortEnabled);
    w->setModel(m_model);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
}
