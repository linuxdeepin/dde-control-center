// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundmodel.h"

#include <DIconButton>
#include <DSysInfo>
#include <DToolButton>

#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QHBoxLayout>
#include <QLoggingCategory>
#include <QStandardItemModel>

Q_LOGGING_CATEGORY(DdcSoundModel, "dcc-sound-model")

Q_DECLARE_METATYPE(const Port *)

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

SoundLabel::SoundLabel(QWidget *parent)
    : QLabel(parent)
    , m_mute(false)
    , m_btn(new DTK_WIDGET_NAMESPACE::DToolButton(this))
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_btn);

    connect(m_btn, &DIconButton::clicked, this, [this]() {
        this->m_mute = !this->m_mute;
        Q_EMIT clicked(this->m_mute);
    });
}

void SoundLabel::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    m_mute = !m_mute;
    Q_EMIT clicked(m_mute);
}

void SoundLabel::setIcon(const QIcon &icon)
{
    m_btn->setIcon(icon);
}

void SoundLabel::setIconSize(const QSize &size)
{
    m_btn->setIconSize(size);
}

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
    , m_soundEffectMapBattery{}
    , m_inputVisibled(false)
    , m_outputVisibled(false)
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
}

SoundModel::~SoundModel()
{
    for (Port *port : m_ports) {
        if (port)
            port->deleteLater();
    }
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

void SoundModel::setPort(const Port *port)
{
    Q_EMIT setPortChanged(port);
}

void SoundModel::addPort(Port *port)
{
    if (!containsPort(port)) {
        m_ports.append(port);

        if (port->direction() == Port::Out) {
            m_outputPorts.append(port);
        } else {
            m_inputPorts.append(port);
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
        } else {
            m_inputPorts.removeOne(port);
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

void SoundModel::setAudioServer(const QString &audioServer)
{
    if (m_audioServer != audioServer) {
        m_audioServer = audioServer;
        Q_EMIT curAudioServerChanged(audioServer);
    }
}

void Port::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        Q_EMIT idChanged(id);
    }
}

void Port::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Port::setCardName(const QString &cardName)
{
    if (cardName != m_cardName) {
        m_cardName = cardName;
        Q_EMIT cardNameChanged(cardName);
    }
}

void Port::setIsActive(bool isActive)
{
    if (isActive != m_isActive) {
        m_isActive = isActive;
        if (m_direction == Port::In)
            Q_EMIT isInputActiveChanged(isActive);
        else
            Q_EMIT isOutputActiveChanged(isActive);
    }
}

void Port::setDirection(const Direction &direction)
{
    if (direction != m_direction) {
        m_direction = direction;
        Q_EMIT directionChanged(direction);
    }
}

void Port::setCardId(const uint &cardId)
{
    if (cardId != m_cardId) {
        m_cardId = cardId;
        Q_EMIT cardIdChanged(cardId);
    }
}

void Port::setEnabled(const bool enabled)
{
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT currentPortEnabled(enabled);
    }
}

void Port::setIsBluetoothPort(const bool isBlue)
{
    if (m_isBluetoothPort != isBlue) {
        m_isBluetoothPort = isBlue;
        Q_EMIT currentBluetoothPortChanged(isBlue);
    }
}
