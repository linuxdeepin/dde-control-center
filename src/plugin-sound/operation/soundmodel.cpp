// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundmodel.h"

#include <DSysInfo>

#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QLoggingCategory>
#include <QStandardItemModel>

Q_LOGGING_CATEGORY(DdcSoundModel, "dcc-sound-model")

const static Dtk::Core::DSysInfo::UosType UosType = Dtk::Core::DSysInfo::uosType();
const static bool IsServerSystem = (Dtk::Core::DSysInfo::UosServer == UosType); //是否是服务器版

static const QMap<DDesktopServices::SystemSoundEffect, QString> SOUND_EFFECT_MAP{
    { DDesktopServices::SystemSoundEffect::SSE_Notifications, "message" },
    { DDesktopServices::SystemSoundEffect::SEE_Screenshot, "camera-shutter" },
    { DDesktopServices::SystemSoundEffect::SSE_EmptyTrash, "trash-empty" },
    { DDesktopServices::SystemSoundEffect::SSE_SendFileComplete, "x-deepin-app-sent-to-desktop" },
    { DDesktopServices::SystemSoundEffect::SSE_BootUp, "desktop-login" },
    { DDesktopServices::SystemSoundEffect::SSE_Shutdown, "system-shutdown" },
    { DDesktopServices::SystemSoundEffect::SSE_Logout, "desktop-logout" },
    { DDesktopServices::SystemSoundEffect::SSE_WakeUp, "suspend-resume" },
    { DDesktopServices::SystemSoundEffect::SSE_VolumeChange, "audio-volume-change" },
    { DDesktopServices::SystemSoundEffect::SSE_LowBattery, "power-unplug-battery-low" },
    { DDesktopServices::SystemSoundEffect::SSE_PlugIn, "power-plug" },
    { DDesktopServices::SystemSoundEffect::SSE_PlugOut, "power-unplug" },
    { DDesktopServices::SystemSoundEffect::SSE_DeviceAdded, "device-added" },
    { DDesktopServices::SystemSoundEffect::SSE_DeviceRemoved, "device-removed" },
    { DDesktopServices::SystemSoundEffect::SSE_Error, "dialog-error" }
};

SoundModel::SoundModel(QObject *parent)
    : QObject(parent)
    , m_speakerOn(true)
    , m_microphoneOn(true)
    , m_enableSoundEffect(false)
    , m_isLaptop(false)
    , m_speakerVolume(75)
    , m_speakerBalance(0)
    , m_microphoneVolume(75)
    , m_maxUIVolume(0.0)
    , m_waitSoundReceiptTime(0)
#ifndef DCC_DISABLE_FEEDBACK
    , m_microphoneFeedback(50)
#endif
    , m_audioCards("")
    , m_currentBluetoothMode("")
    , m_inputVisibled(false)
    , m_outputVisibled(false)
    , m_outPutPortComboIndex(0)
    , m_outPutPortComboEnable(true)
    , m_inPutPortComboIndex(0)
    , m_inPutPortComboEnable(true)
    , m_soundEffectsModel(new SoundEffectsModel(this))
    , m_soundInputDeviceModel(new SoundDeviceModel(this))
    , m_soundOutputDeviceModel(new SoundDeviceModel(this))
    , m_audioServerModel(new AudioServerModel(this))
    , m_inPutPortCount(0)
    , m_outPutCount(0)
    , m_audioMono(false)
    , m_showBluetoothMode(false)
    , m_showInputBluetoothMode(false)
{
    m_soundEffectMapBattery = {
        { tr("Boot up"), DDesktopServices::SSE_BootUp },
        { tr("Shut down"), DDesktopServices::SSE_Shutdown },
        { tr("Log out"), DDesktopServices::SSE_Logout },
        { tr("Wake up"), DDesktopServices::SSE_WakeUp },
        { tr("Volume +/-"), DDesktopServices::SSE_VolumeChange },
        { tr("Notification"), DDesktopServices::SSE_Notifications },
        { tr("Low battery"), DDesktopServices::SSE_LowBattery },
        { tr("Send icon in Launcher to Desktop"), DDesktopServices::SSE_SendFileComplete },
        { tr("Empty Trash"), DDesktopServices::SSE_EmptyTrash },
        { tr("Plug in"), DDesktopServices::SSE_PlugIn },
        { tr("Plug out"), DDesktopServices::SSE_PlugOut },
        { tr("Removable device connected"), DDesktopServices::SSE_DeviceAdded },
        { tr("Removable device removed"), DDesktopServices::SSE_DeviceRemoved },
        { tr("Error"), DDesktopServices::SSE_Error },
    };

    m_soundEffectMapPower = {
        { tr("Boot up"), DDesktopServices::SSE_BootUp },
        { tr("Shut down"), DDesktopServices::SSE_Shutdown },
        { tr("Log out"), DDesktopServices::SSE_Logout },
        { tr("Wake up"), DDesktopServices::SSE_WakeUp },
        { tr("Volume +/-"), DDesktopServices::SSE_VolumeChange },
        { tr("Notification"), DDesktopServices::SSE_Notifications },
        { tr("Send icon in Launcher to Desktop"), DDesktopServices::SSE_SendFileComplete },
        { tr("Empty Trash"), DDesktopServices::SSE_EmptyTrash },
        { tr("Removable device connected"), DDesktopServices::SSE_DeviceAdded },
        { tr("Removable device removed"), DDesktopServices::SSE_DeviceRemoved },
        { tr("Error"), DDesktopServices::SSE_Error },
    };

    if (IsServerSystem) {
        m_soundEffectMapBattery.removeOne({ tr("Wake up"), DDesktopServices::SSE_WakeUp });
        m_soundEffectMapPower.removeOne({ tr("Wake up"), DDesktopServices::SSE_WakeUp });
    }
    qmlRegisterType<SoundDeviceModel>("SoundDeviceModel", 1, 0, "SoundDeviceModel");
}

SoundModel::~SoundModel()
{
    for (Port *port : m_ports) {
        if (port)
            port->deleteLater();
    }
}

void SoundModel::updatePortCombo()
{
    QStringList outPutPortCombo;
    QStringList inPutPortCombo;

    for (Port* port : m_ports) {
        if (port->isEnabled()) {
            switch (port->direction()) {
            case Port::In:
                inPutPortCombo.append(port->name() + "(" + port->cardName() + ")");
                break;
            case Port::Out:
                outPutPortCombo.append(port->name() + "(" + port->cardName() + ")");
                break;
            }
        }
    }

    setInPutPortCombo(inPutPortCombo);
    setOutPutPortCombo(outPutPortCombo);
}

int SoundModel::inPutPortComboIndex() const
{
    return m_inPutPortComboIndex;
}

void SoundModel::setInPutPortComboIndex(int newInPutPortComboIndex)
{
    if (m_inPutPortComboIndex == newInPutPortComboIndex)
        return;
    m_inPutPortComboIndex = newInPutPortComboIndex;
    emit inPutPortComboIndexChanged();
}

QStringList SoundModel::inPutPortCombo() const
{
    return m_inPutPortCombo;
}

void SoundModel::setInPutPortCombo(const QStringList &newInPutPortCombo)
{
    if (m_inPutPortCombo == newInPutPortCombo)
        return;
    m_inPutPortCombo = newInPutPortCombo;
    emit inPutPortComboChanged();
}

int SoundModel::outPutPortComboIndex() const
{
    return m_outPutPortComboIndex;
}

void SoundModel::setOutPutPortComboIndex(int newOutPutPortComboIndex)
{
    if (newOutPutPortComboIndex < 0 || m_outPutPortComboIndex == newOutPutPortComboIndex)
        return;
    m_outPutPortComboIndex = newOutPutPortComboIndex;
    emit outPutPortComboIndexChanged();
}

void SoundModel::setSpeakerOn(bool speakerOn)
{
    if (speakerOn != m_speakerOn) {
        m_speakerOn = speakerOn;

        Q_EMIT speakerOnChanged(speakerOn);
    }
}

void SoundModel::setPortEnable(bool enable)
{
    if (enable != m_portEnable)
        m_portEnable = enable;
    Q_EMIT isPortEnableChanged(enable);
}

void SoundModel::setReduceNoise(bool reduceNoise)
{
    if (reduceNoise != m_reduceNoise) {
        m_reduceNoise = reduceNoise;
        Q_EMIT reduceNoiseChanged(reduceNoise);
    }
}

void SoundModel::setPausePlayer(bool pausePlayer)
{
    if (pausePlayer != m_pausePlayer) {
        m_pausePlayer = pausePlayer;
        Q_EMIT pausePlayerChanged(pausePlayer);
    }
}

void SoundModel::setMicrophoneOn(bool microphoneOn)
{
    if (microphoneOn != m_microphoneOn) {
        m_microphoneOn = microphoneOn;

        Q_EMIT microphoneOnChanged(microphoneOn);
    }
}

void SoundModel::setSpeakerBalance(double speakerBalance)
{
    if (!qFuzzyCompare(speakerBalance, m_speakerBalance)) {


        m_speakerBalance = speakerBalance;

        Q_EMIT speakerBalanceChanged(speakerBalance);
    }
}

void SoundModel::setMicrophoneVolume(double microphoneVolume)
{
    if (!qFuzzyCompare(microphoneVolume, m_microphoneVolume)) {
        m_microphoneVolume = microphoneVolume;

        Q_EMIT microphoneVolumeChanged(microphoneVolume);
    }
}
#ifndef DCC_DISABLE_FEEDBACK
void SoundModel::setMicrophoneFeedback(double microphoneFeedback)
{
    if (!qFuzzyCompare(microphoneFeedback, m_microphoneFeedback)) {
        m_microphoneFeedback = microphoneFeedback;
        Q_EMIT microphoneFeedbackChanged(microphoneFeedback);
    }
}
#endif

void SoundModel::setPort(Port *port)
{
    Q_EMIT setPortChanged(port);
}

void SoundModel::addPort(Port *port)
{
    if (!containsPort(port)) {
        m_ports.append(port);

        if (port->direction() == Port::Out) {
            m_outputPorts.append(port);
            setOutPutCount(static_cast<int>(m_outputPorts.count()));
            if (port->isEnabled()) {
                m_outPutPortCombo.append(port->name() + "(" + port->cardName() + ")");
            }

            m_soundOutputDeviceModel->addData(port);
        } else {
            m_inputPorts.append(port);
            setInPutPortCount(static_cast<int>(m_inputPorts.count()));
            if (port->isEnabled()) {
                m_inPutPortCombo.append(port->name() + "(" + port->cardName() + ")");
            }
            m_soundInputDeviceModel->addData(port);
            emit inPutPortComboChanged();
        }

        Q_EMIT portAdded(port);
        Q_EMIT soundDeviceStatusChanged();
    }
}

void SoundModel::removePort(const QString &portId, const uint &cardId)
{
    Port *port = findPort(portId, cardId);
    if (port) {
        Q_EMIT portRemoved(portId, cardId, port->direction());
        m_ports.removeOne(port);

        if (port->direction() == Port::Out) {
            m_outputPorts.removeOne(port);
            setOutPutCount(static_cast<int>(m_outputPorts.count()));
            m_outPutPortCombo.removeOne(port->name() + "(" + port->cardName() + ")");

            m_soundOutputDeviceModel->removeData(port);

        } else {
            m_inputPorts.removeOne(port);
            setInPutPortCount(static_cast<int>(m_inputPorts.count()));
            m_inPutPortCombo.removeOne(port->name() + "(" + port->cardName() + ")");
            m_soundInputDeviceModel->removeData(port);
            emit inPutPortComboChanged();
        }
        port->deleteLater();
    }
}

bool SoundModel::containsPort(const Port *port)
{
    return findPort(port->id(), port->cardId()) != nullptr;
}

Port *SoundModel::findPort(const QString &portId, const uint &cardId) const
{
    auto res = std::find_if(m_ports.cbegin(), m_ports.end(), [=](const Port *data) -> bool {
        return ((data->id() == portId) && (data->cardId() == cardId));
    });

    if (res != m_ports.cend()) {
        return *res;
    }

    return nullptr;
}

QList<Port *> SoundModel::ports() const
{
    return m_ports;
}

void SoundModel::setSpeakerVolume(double speakerVolume)
{
    if (!qFuzzyCompare(m_speakerVolume, speakerVolume)) {
        m_speakerVolume = speakerVolume;
        Q_EMIT speakerVolumeChanged(speakerVolume);
    }
}

void SoundModel::setMaxUIVolume(double value)
{
    double val = qRound(value * 10) / 10.0;
    if (!qFuzzyCompare(val, m_maxUIVolume)) {
        m_maxUIVolume = val;
        Q_EMIT maxUIVolumeChanged(val);
    }
}

QDBusObjectPath SoundModel::defaultSource() const
{
    return m_defaultSource;
}

void SoundModel::setDefaultSource(const QDBusObjectPath &defaultSource)
{
    m_defaultSource = defaultSource;

    Q_EMIT defaultSourceChanged(m_defaultSource);
}

QDBusObjectPath SoundModel::defaultSink() const
{
    return m_defaultSink;
}

void SoundModel::setDefaultSink(const QDBusObjectPath &defaultSink)
{
    m_defaultSink = defaultSink;

    Q_EMIT defaultSinkChanged(m_defaultSink);
}

QString SoundModel::audioCards() const
{
    return m_audioCards;
}

void SoundModel::setAudioCards(const QString &audioCards)
{
    m_audioCards = audioCards;

    Q_EMIT audioCardsChanged(m_audioCards);
}

SoundEffectList SoundModel::soundEffectMap() const
{
    if (isLaptop()) { // 笔记本
        return m_soundEffectMapBattery;
    } else { // 台式机
        return m_soundEffectMapPower;
    }
}

void SoundModel::setEffectData(DDesktopServices::SystemSoundEffect effect, const bool enable)
{
    if (m_soundEffectData[effect] == enable)
        return;

    m_soundEffectData[effect] = enable;

    Q_EMIT soundEffectDataChanged(effect, enable);
}

bool SoundModel::queryEffectData(DDesktopServices::SystemSoundEffect effect)
{
    return m_soundEffectData[effect];
}

void SoundModel::setEnableSoundEffect(bool enableSoundEffect)
{
    if (m_enableSoundEffect == enableSoundEffect)
        return;

    m_enableSoundEffect = enableSoundEffect;

    Q_EMIT enableSoundEffectChanged(enableSoundEffect);
}

void SoundModel::updateSoundEffectPath(DDesktopServices::SystemSoundEffect effect,
                                       const QString &path)
{
    m_soundEffectPaths[effect] = path;
}

const QString SoundModel::soundEffectPathByType(DDesktopServices::SystemSoundEffect effect)
{
    return m_soundEffectPaths[effect];
}

const QString SoundModel::getNameByEffectType(DDesktopServices::SystemSoundEffect effect) const
{
    return SOUND_EFFECT_MAP.value(effect);
}

DDesktopServices::SystemSoundEffect SoundModel::getEffectTypeByGsettingName(const QString &name)
{
    return SOUND_EFFECT_MAP.key(name);
}

bool SoundModel::checkSEExist(const QString &name)
{
    return SOUND_EFFECT_MAP.values().contains(name);
}

bool SoundModel::isLaptop() const
{
    return m_isLaptop;
}

void SoundModel::setIsLaptop(bool isLaptop)
{
    if (isLaptop == m_isLaptop)
        return;

    m_isLaptop = isLaptop;

    Q_EMIT isLaptopChanged(isLaptop);
}

bool SoundModel::isIncreaseVolume() const
{
    return m_increaseVolume;
}

void SoundModel::setIncreaseVolume(bool value)
{
    if (m_increaseVolume != value) {
        m_increaseVolume = value;
        Q_EMIT increaseVolumeChanged(value);
    }
}

void SoundModel::setBluetoothAudioModeOpts(const QStringList &modes)
{
    if (modes != m_bluetoothModeOpts) {
        m_bluetoothModeOpts = modes;
        Q_EMIT bluetoothModeOptsChanged(modes);
    }
}

void SoundModel::setCurrentBluetoothAudioMode(const QString &mode)
{
    if (mode != m_currentBluetoothMode) {
        m_currentBluetoothMode = mode;
        Q_EMIT bluetoothModeChanged(mode);
    }
}

void SoundModel::setWaitSoundReceiptTime(const int receiptTime)
{
    // 配置端⼝切换延时时间
    if (m_waitSoundReceiptTime != receiptTime) {
        qCDebug(DdcSoundModel) << "Sound Receopt Time is: " << receiptTime;
        m_waitSoundReceiptTime = receiptTime;
    }
}

void SoundModel::setAudioServerChangedState(const bool state)
{
    if (m_audioServerStatus != state) {
        m_audioServerStatus = state;
        Q_EMIT onSetAudioServerFinish(state);
    }
}

void SoundModel::updateSoundEffectsModel()
{
    m_soundEffectsModel->clearData();
    SoundEffectList list = soundEffectMap();

    for (std::pair<QString, DDesktopServices::SystemSoundEffect> item : list) {
        if (m_soundEffectData.contains(item.second) && m_soundEffectPaths.contains(item.second)) {
            SoundEffectsData* data = new SoundEffectsData;
            data->setName(item.first);
            data->setSystemSoundEffect(item.second);
            data->setChecked(m_soundEffectData[item.second]);
            data->setPath(m_soundEffectPaths[item.second]);
            data->setAniIconPath("");
            m_soundEffectsModel->addData(data);
        }
    }
}

QString SoundModel::getSoundEffectsType(int index)
{
    SoundEffectsData* data = m_soundEffectsModel->getSystemSoundEffect(index);
    return data ? getNameByEffectType(data->systemSoundEffect()) : "";
}

void SoundModel::setSoundEffectEnable(int index, bool enable)
{
    m_soundEffectsModel->updateSoundEffectsData(index, enable);
}

void SoundModel::initSoundDeviceModel(Port::Direction direction)
{
    SoundDeviceModel *model =
            direction == Port::In ? soundInputDeviceModel() : soundOutputDeviceModel();
    QList<Port *> ports = direction == Port::In ? m_inputPorts : m_outputPorts;
    model->clearData();

    for (Port *port : ports) {
        model->addData(port);
    }
}

Port *SoundModel::getSoundDeviceData(int index, int portType)
{
    SoundDeviceModel *soundDeviceModel = portType == Port::In ? soundInputDeviceModel() : soundOutputDeviceModel();
    return soundDeviceModel ? soundDeviceModel->getSoundDeviceData(index) : nullptr;
}

void SoundModel::updateSoundDeviceModel(Port *port)
{
    SoundDeviceModel *soundDeviceModel =
            port->direction() == Port::In ? soundInputDeviceModel() : soundOutputDeviceModel();

    soundDeviceModel->updateSoundDeviceData(port);
}

void SoundModel::updateAllDeviceModel()
{
    m_soundOutputDeviceModel->updateAllSoundDeviceData();
    m_soundInputDeviceModel->updateAllSoundDeviceData();
}

void SoundModel::updateActiveComboIndex()
{
    setInPutPortComboIndex(m_soundInputDeviceModel->getCurrentIndex());
    setOutPutPortComboIndex(m_soundOutputDeviceModel->getCurrentIndex());
}

void SoundModel::setOutPutCount(int newOutPutCount)
{
    if (m_outPutCount == newOutPutCount)
        return;
    m_outPutCount = newOutPutCount;
    emit outPutCountChanged();
}

void SoundModel::updatePlayAniIconPath(int index, const QString &newPlayAniIconPath)
{
    m_soundEffectsModel->updateSoundEffectsAniIcon(index, newPlayAniIconPath);
}

AudioServerModel *SoundModel::audioServerModel() const
{
    return m_audioServerModel;
}

void SoundModel::setAudioServerModel(AudioServerModel *newAudioServerModel)
{
    m_audioServerModel = newAudioServerModel;
}

void SoundModel::addAudioServerData(const AudioServerData &newAudioServerData)
{
    m_audioServerModel->addData(newAudioServerData);
}

bool SoundModel::audioMono() const
{
    return m_audioMono;
}

void SoundModel::setAudioMono(bool newAudioMono)
{
    m_audioMono = newAudioMono;
    emit audioMonoChanged();
}

bool SoundModel::showBluetoothMode() const
{
    return m_showBluetoothMode;
}

void SoundModel::setShowBluetoothMode(bool newShowBluetoothMode)
{
    if (m_showBluetoothMode == newShowBluetoothMode)
        return;
    m_showBluetoothMode = newShowBluetoothMode;
    emit showBluetoothModeChanged();
}

bool SoundModel::outPutPortComboEnable() const
{
    return m_outPutPortComboEnable;
}

void SoundModel::setOutPutPortComboEnable(bool newOutPutPortComboEnable)
{
    if (m_outPutPortComboEnable == newOutPutPortComboEnable)
        return;
    m_outPutPortComboEnable = newOutPutPortComboEnable;
    emit outPutPortComboEnableChanged();
}

bool SoundModel::inPutPortComboEnable() const
{
    return m_inPutPortComboEnable;
}

void SoundModel::setInPutPortComboEnable(bool newInPutPortComboEnable)
{
    if (m_inPutPortComboEnable == newInPutPortComboEnable)
        return;
    m_inPutPortComboEnable = newInPutPortComboEnable;
    emit inPutPortComboEnableChanged();
}

void SoundModel::setInPutPortCount(int newInPutPortCount)
{
    if (m_inPutPortCount == newInPutPortCount)
        return;
    m_inPutPortCount = newInPutPortCount;
    emit inPutPortCountChanged();
}

int SoundModel::outPutCount() const
{
    return m_outPutCount;
}

int SoundModel::inPutPortCount() const
{
    return m_inPutPortCount;
}

SoundDeviceModel *SoundModel::soundOutputDeviceModel() const
{
    return m_soundOutputDeviceModel;
}

void SoundModel::setSoundOutputDeviceModel(SoundDeviceModel *newSoundOutputDeviceModel)
{
    m_soundOutputDeviceModel = newSoundOutputDeviceModel;
}

int SoundModel::getDeviceManagerRowCount(int portType) const
{
    SoundDeviceModel* soundDeviceModel = portType == Port::In ? soundInputDeviceModel() : soundOutputDeviceModel();
    return soundDeviceModel ? soundDeviceModel->getRowCount() : 0;
}

SoundDeviceModel *SoundModel::soundInputDeviceModel() const
{
    return m_soundInputDeviceModel;
}

void SoundModel::setSoundInputDeviceModel(SoundDeviceModel *newSoundInputDeviceModel)
{
    m_soundInputDeviceModel = newSoundInputDeviceModel;
}


SoundEffectsModel* SoundModel::soundEffectsModel() const
{
    return m_soundEffectsModel;
}


int SoundModel::getSoundEffectsRowCount() const
{
    return m_soundEffectsModel ? m_soundEffectsModel->getRowCount() : 0;
}

void SoundModel::setAudioServer(const QString &audioServer)
{
    if (m_audioServer != audioServer) {
        m_audioServer = audioServer;
        Q_EMIT curAudioServerChanged(audioServer);

        m_audioServerModel->updateCheckedService(audioServer);
    }
}

void SoundModel::setOutPutPortCombo(const QStringList& outPutPort)
{
    m_outPutPortCombo = outPutPort;
    Q_EMIT outPutPortComboChanged(m_outPutPortCombo);
}

QString SoundModel::getListName(int index) const
{
    Q_UNUSED(index)
    return QString();
}

bool SoundModel::showInputBluetoothMode() const
{
    return m_showInputBluetoothMode;
}

void SoundModel::setShowInputBluetoothMode(bool newShowInputBluetoothMode)
{
    if (m_showInputBluetoothMode == newShowInputBluetoothMode)
        return;
    m_showInputBluetoothMode = newShowInputBluetoothMode;
    emit showInputBluetoothModeChanged();
}

