//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_SOUND_SOUNDMODEL_H
#define DCC_SOUND_SOUNDMODEL_H

#include <dtkwidget_global.h>
#include <dtkgui_global.h>
#include <DDesktopServices>
#include <QObject>
#include <QMap>
#include <QLabel>
#include <QDBusObjectPath>
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
class DToolButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QString;
QT_END_NAMESPACE

using SoundEffectList = QList<std::pair<QString, DDesktopServices::SystemSoundEffect>>;

class Port : public QObject
{
    Q_OBJECT
public:
    enum Direction {
        Out = 1,
        In = 2
    };

    explicit Port(QObject * parent) : QObject(parent),m_id(""), m_name(""), m_cardName(""), m_cardId(0), m_isActive(false), m_enabled(false), m_isBluetoothPort(false), m_direction(Out){}
    virtual ~Port() {}

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline QString cardName() const { return m_cardName; }
    void setCardName(const QString &cardName);

    inline bool isActive() const { return m_isActive; }
    void setIsActive(bool isActive);

    inline Direction direction() const { return m_direction; }
    void setDirection(const Direction &direction);

    inline uint cardId() const { return m_cardId; }
    void setCardId(const uint &cardId);

    inline bool isEnabled() const { return m_enabled; }
    void setEnabled(const bool enabled);

    inline bool isBluetoothPort() const  { return m_isBluetoothPort; }
    void setIsBluetoothPort(const bool isBlue);

Q_SIGNALS:
    void idChanged(QString id) const;
    void nameChanged(QString name) const;
    void cardNameChanged(QString name) const;
    void isInputActiveChanged(bool active) const;
    void isOutputActiveChanged(bool active) const;
    void directionChanged(Direction direction) const;
    void cardIdChanged(uint cardId) const;
    void currentPortEnabled(bool enable) const;
    void currentBluetoothPortChanged(bool isBlue) const;

private:
    QString m_id;
    QString m_name;
    QString m_cardName;
    uint m_cardId;
    bool m_isActive;
    bool m_enabled;
    bool m_isBluetoothPort;
    Direction m_direction;
};

class SoundLabel : public QLabel
{
    Q_OBJECT
public:
    explicit SoundLabel(QWidget *parent = nullptr);
    void mouseReleaseEvent(QMouseEvent *e) override;
    virtual ~SoundLabel() {}
    void setIcon(const QIcon &icon);
    void setIconSize(const QSize &size);

private:
    bool m_mute;
    DTK_WIDGET_NAMESPACE::DToolButton *m_btn;

Q_SIGNALS:
    void clicked(bool checked);
};

class SoundModel : public QObject
{
    Q_OBJECT
public:
    explicit SoundModel(QObject *parent = 0);
    ~SoundModel();

    inline bool speakerOn() const { return m_speakerOn; }
    void setSpeakerOn(bool speakerOn);

    inline bool isPortEnable() const { return m_portEnable; }
    void setPortEnable(bool enable);

    inline bool reduceNoise() const { return m_reduceNoise; }
    void setReduceNoise(bool reduceNoise);

    inline bool pausePlayer() const { return m_pausePlayer; }
    void setPausePlayer(bool reduceNoise);

    inline bool microphoneOn() const { return m_microphoneOn; }
    void setMicrophoneOn(bool microphoneOn);

    inline double speakerBalance() const { return m_speakerBalance; }
    void setSpeakerBalance(double speakerBalance);

    inline double microphoneVolume() const { return m_microphoneVolume; }
    void setMicrophoneVolume(double microphoneVolume);

#ifndef DCC_DISABLE_FEEDBACK
    inline double microphoneFeedback() const { return m_microphoneFeedback; }
    void setMicrophoneFeedback(double microphoneFeedback);
#endif

    void setPort(const Port *port);
    void addPort(Port *port);
    void removePort(const QString &portId, const uint &cardId);
    bool containsPort(const Port *port);
    Port *findPort(const QString &portId, const uint &cardId) const;
    QList<Port *> ports() const;

    inline double speakerVolume() const { return m_speakerVolume; }
    void setSpeakerVolume(double speakerVolume);

    QDBusObjectPath defaultSource() const;
    void setDefaultSource(const QDBusObjectPath &defaultSource);

    QDBusObjectPath defaultSink() const;
    void setDefaultSink(const QDBusObjectPath &defaultSink);

    QString audioCards() const;
    void setAudioCards(const QString &audioCards);

    inline double MaxUIVolume() const { return m_maxUIVolume; }
    void setMaxUIVolume(double value);

    SoundEffectList soundEffectMap() const;

    void setEffectData(DDesktopServices::SystemSoundEffect effect, const bool enable);
    bool queryEffectData(DDesktopServices::SystemSoundEffect effect);

    bool enableSoundEffect() const { return m_enableSoundEffect; }
    void setEnableSoundEffect(bool enableSoundEffect);

    void updateSoundEffectPath(DDesktopServices::SystemSoundEffect effect, const QString &path);
    inline QMap<DDesktopServices::SystemSoundEffect, QString> soundEffectPaths() { return m_soundEffectPaths; }
    const QString soundEffectPathByType(DDesktopServices::SystemSoundEffect effect);

    const QString getNameByEffectType(DDesktopServices::SystemSoundEffect effect) const;
    DDesktopServices::SystemSoundEffect getEffectTypeByGsettingName(const QString &name);

    bool checkSEExist(const QString &name); // SE: Sound Effect

    bool isLaptop() const;
    void setIsLaptop(bool isLaptop);

    bool isIncreaseVolume() const;
    void setIncreaseVolume(bool value);
    void initMicroPhone() { Q_EMIT microphoneOnChanged(m_microphoneOn); }
    void initSpeaker() { Q_EMIT speakerOnChanged(m_speakerOn); }

    inline QStringList bluetoothAudioModeOpts() { return m_bluetoothModeOpts; }
    void setBluetoothAudioModeOpts(const QStringList &modes);

    // 设置当前蓝牙耳机模式
    inline QString currentBluetoothAudioMode() { return m_currentBluetoothMode; }
    void setCurrentBluetoothAudioMode(const QString &mode);

    // 配置等待
    inline int currentWaitSoundReceiptTime() { return m_waitSoundReceiptTime; }
    void setWaitSoundReceiptTime(const int receiptTime);

    // 设置音频框架
    inline QString audioServer() const { return m_audioServer; }
    void setAudioServer(const QString &serverName);

    // 音频框架切换的状态
    inline bool audioServerChangedState() const { return m_audioServerStatus; }
    void setAudioServerChangedState(const bool state);

Q_SIGNALS:
    void speakerOnChanged(bool speakerOn) const;
    void microphoneOnChanged(bool microphoneOn) const;
    void soundEffectOnChanged(bool soundEffectOn) const;
    void speakerVolumeChanged(double speakerVolume) const;
    void speakerBalanceChanged(double speakerBalance) const;
    void microphoneVolumeChanged(double microphoneVolume) const;
    void defaultSourceChanged(const QDBusObjectPath &defaultSource) const;
    void defaultSinkChanged(const QDBusObjectPath &defaultSink) const;
    void audioCardsChanged(const QString &audioCards) const;
    void maxUIVolumeChanged(double value) const;
    void increaseVolumeChanged(bool value) const;
    void reduceNoiseChanged(bool reduceNoise) const;
    void pausePlayerChanged(bool pausePlayer) const;
    void isPortEnableChanged(bool enable) const;
    void bluetoothModeOptsChanged(const QStringList &modeOpts) const;
    void bluetoothModeChanged(const QString &mode);

    void setPortChanged(const Port* port) const;
    //查询是否可用
    void requestSwitchEnable(unsigned int cardId,QString cardName);

    //声音输入设备是否可见
    void inputDevicesVisibleChanged(QString name, bool flag);
    //声音输出设备是否可见
    void outputDevicesVisibleChanged(QString name, bool flag);

    // 音频框架设置完成
    void onSetAudioServerFinish(bool value);
    // 当前音频框架切换的信号
    void curAudioServerChanged(const QString &audioFrame);

#ifndef DCC_DISABLE_FEEDBACK
    void microphoneFeedbackChanged(double microphoneFeedback) const;
#endif
    void portAdded(const Port *port);
    void portRemoved(const QString & portId, const uint &cardId, const Port::Direction &direction);
    void soundDeviceStatusChanged();
    void soundEffectDataChanged(DDesktopServices::SystemSoundEffect effect, const bool enable);
    void enableSoundEffectChanged(bool enableSoundEffect);
    void isLaptopChanged(bool isLaptop);

private:
    QString m_audioServer;     // 当前使用音频框架
    bool m_audioServerStatus{true};  // 设置音频时的状态
    bool m_speakerOn;
    bool m_microphoneOn;
    bool m_enableSoundEffect;
    bool m_isLaptop;
    bool m_increaseVolume{false};
    bool m_reduceNoise{true};
    bool m_pausePlayer{true};
    bool m_portEnable{false};
    double m_speakerVolume;
    double m_speakerBalance;
    double m_microphoneVolume;
    double m_maxUIVolume;
    int m_waitSoundReceiptTime;

#ifndef DCC_DISABLE_FEEDBACK
    double m_microphoneFeedback;
#endif
    QList<Port *> m_ports;
    QList<Port *> m_inputPorts;
    QList<Port *> m_outputPorts;
    Port *m_activePort;

    QDBusObjectPath m_defaultSource;
    QDBusObjectPath m_defaultSink;
    QString m_audioCards;
    QStringList m_bluetoothModeOpts;
    QString m_currentBluetoothMode;

    SoundEffectList m_soundEffectMapPower;
    SoundEffectList m_soundEffectMapBattery;
    QMap<DDesktopServices::SystemSoundEffect, bool> m_soundEffectData;
    QMap<DDesktopServices::SystemSoundEffect, QString> m_soundEffectPaths;

    bool m_inputVisibled;
    bool m_outputVisibled;
};

#endif // DCC_SOUND_SOUNDMODEL_H
