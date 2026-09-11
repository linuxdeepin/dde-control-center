// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_SOUND_SOUNDMODEL_H
#define DCC_SOUND_SOUNDMODEL_H

#include <dtkgui_global.h>
#include <DDesktopServices>
#include <QObject>
#include <QMap>
#include <QDBusObjectPath>
#include <QtQml/qqml.h>

#include "soundeffectsmodel.h"
#include "soundDeviceModel.h"
#include "audioservermodel.h"
#include "port.h"

DGUI_USE_NAMESPACE


QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QString;
QT_END_NAMESPACE

using SoundEffectList = QList<std::pair<QString, DDesktopServices::SystemSoundEffect>>;

class SoundModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double speakerVolume READ speakerVolume  NOTIFY speakerVolumeChanged)
    Q_PROPERTY(bool increaseVolume READ isIncreaseVolume NOTIFY increaseVolumeChanged)
    Q_PROPERTY(double speakerBalance READ speakerBalance NOTIFY speakerBalanceChanged)
    Q_PROPERTY(QStringList outPutPortCombo READ outPutPortCombo NOTIFY outPutPortComboChanged)
    Q_PROPERTY(bool pausePlayer READ pausePlayer NOTIFY pausePlayerChanged)

    Q_PROPERTY(double microphoneVolume READ microphoneVolume NOTIFY microphoneVolumeChanged)
    Q_PROPERTY(bool reduceNoise READ reduceNoise NOTIFY reduceNoiseChanged)
    Q_PROPERTY(int outPutPortComboIndex READ outPutPortComboIndex NOTIFY outPutPortComboIndexChanged FINAL)
    Q_PROPERTY(double microphoneFeedback READ microphoneFeedback NOTIFY microphoneFeedbackChanged)
    Q_PROPERTY(bool enableSoundEffect READ enableSoundEffect NOTIFY enableSoundEffectChanged)
    Q_PROPERTY(QStringList inPutPortCombo READ inPutPortCombo WRITE setInPutPortCombo NOTIFY inPutPortComboChanged FINAL)
    Q_PROPERTY(int inPutPortComboIndex READ inPutPortComboIndex WRITE setInPutPortComboIndex NOTIFY inPutPortComboIndexChanged FINAL)
    Q_PROPERTY(int inPutPortCount READ inPutPortCount NOTIFY inPutPortCountChanged FINAL)
    Q_PROPERTY(int outPutCount READ outPutCount NOTIFY outPutCountChanged FINAL)
    Q_PROPERTY(bool audioServerStatus READ audioServerChangedState NOTIFY onSetAudioServerFinish FINAL)
    Q_PROPERTY(QString audioServer READ audioServer NOTIFY curAudioServerChanged FINAL)
    Q_PROPERTY(bool audioMono READ audioMono NOTIFY audioMonoChanged FINAL)
    Q_PROPERTY(QStringList bluetoothModeOpts READ bluetoothAudioModeOpts NOTIFY bluetoothModeOptsChanged FINAL)
    Q_PROPERTY(QString currentBluetoothAudioMode READ currentBluetoothAudioMode NOTIFY bluetoothModeChanged FINAL)
    Q_PROPERTY(bool showBluetoothMode READ showBluetoothMode NOTIFY showBluetoothModeChanged FINAL)
    Q_PROPERTY(bool showInputBluetoothMode READ showInputBluetoothMode NOTIFY showInputBluetoothModeChanged FINAL)

    Q_PROPERTY(bool outPutPortComboEnable READ outPutPortComboEnable NOTIFY outPutPortComboEnableChanged FINAL)
    Q_PROPERTY(bool inPutPortComboEnable READ inPutPortComboEnable NOTIFY inPutPortComboEnableChanged FINAL)

    Q_PROPERTY(bool speakerOn READ speakerOn NOTIFY speakerOnChanged FINAL)
    Q_PROPERTY(bool microphoneOn READ microphoneOn NOTIFY microphoneOnChanged FINAL)

    QML_NAMED_ELEMENT(SoundModel)
    QML_SINGLETON
public:
    explicit SoundModel(QObject *parent = 0);
    ~SoundModel();

    bool speakerOn() const { return m_speakerOn; }
    void setSpeakerOn(bool speakerOn);

    inline bool isPortEnable() const { return m_portEnable; }
    void setPortEnable(bool enable);

    inline bool reduceNoise() const { return m_reduceNoise; }
    void setReduceNoise(bool reduceNoise);

    inline bool pausePlayer() const { return m_pausePlayer; }
    void setPausePlayer(bool reduceNoise);

    inline bool microphoneOn() const { return m_microphoneOn; }
    void setMicrophoneOn(bool microphoneOn);

    inline double speakerBalance() const { return m_speakerBalance ; }
    void setSpeakerBalance(double speakerBalance);

    inline double microphoneVolume() const { return m_microphoneVolume; }
    void setMicrophoneVolume(double microphoneVolume);

    inline QStringList outPutPortCombo() const { return m_outPutPortCombo;}
    void setOutPutPortCombo(const QStringList& outPutPort);

#ifndef DCC_DISABLE_FEEDBACK
    inline double microphoneFeedback() const { return m_microphoneFeedback; }
    void setMicrophoneFeedback(double microphoneFeedback);
#endif

    void setPort(Port *port);
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

    // 更新系统声效ui数据
    void updateSoundEffectsModel();
    QString getSoundEffectsType(int index);

    void setSoundEffectEnable(int index, bool enable);

    // 初始化输入设备ui数据
    void initSoundDeviceModel(Port::Direction direction);
    Port* getSoundDeviceData(int index, int portType);
    void updateSoundDeviceModel(Port* port);
    void updateAllDeviceModel();
    void updateActiveComboIndex();

    int outPutPortComboIndex() const;
    void setOutPutPortComboIndex(int newOutPutPortComboIndex);
    QStringList inPutPortCombo() const;
    void setInPutPortCombo(const QStringList &newInPutPortCombo);
    int inPutPortComboIndex() const;
    void setInPutPortComboIndex(int newInPutPortComboIndex);

    void updatePortCombo();

    Q_INVOKABLE SoundEffectsModel* soundEffectsModel() const;
    Q_INVOKABLE QString getListName(int index) const;
    Q_INVOKABLE int getSoundEffectsRowCount() const;
    Q_INVOKABLE SoundDeviceModel *soundInputDeviceModel() const;
    void setSoundInputDeviceModel(SoundDeviceModel *newSoundInputDeviceModel);
    Q_INVOKABLE SoundDeviceModel *soundOutputDeviceModel() const;
    void setSoundOutputDeviceModel(SoundDeviceModel *newSoundOutputDeviceModel);
    Q_INVOKABLE int getDeviceManagerRowCount(int portType) const;
    int inPutPortCount() const;
    int outPutCount() const;
    void setInPutPortCount(int newInPutPortCount);
    void setOutPutCount(int newOutPutCount);

    void updatePlayAniIconPath(int index, const QString &newPlayAniIconPath);

    Q_INVOKABLE AudioServerModel *audioServerModel() const;
    void setAudioServerModel(AudioServerModel *newAudioServerModel);
    void addAudioServerData(const AudioServerData &newAudioServerData);

    bool audioMono() const;
    void setAudioMono(bool newAudioMono);

    bool showBluetoothMode() const;
    void setShowBluetoothMode(bool newShowBluetoothMode);

    bool outPutPortComboEnable() const;
    void setOutPutPortComboEnable(bool newOutPutPortComboEnable);

    bool inPutPortComboEnable() const;
    void setInPutPortComboEnable(bool newInPutPortComboEnable);

    bool showInputBluetoothMode() const;
    void setShowInputBluetoothMode(bool newShowInputBluetoothMode);

private:


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

    void setPortChanged(Port* port) const;
    void outPutPortComboChanged(const QStringList &outPutPort) const;
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

    void outPutPortComboIndexChanged();
    void inPutPortComboChanged();
    void inPutPortComboIndexChanged();
    void soundEffectsModelChanged();
    void inPutPortCountChanged();
    void outPutCountChanged();

    void playAniIconPathChanged();

    void audioMonoChanged();

    void showBluetoothModeChanged();

    void outPutPortComboEnableChanged();

    void inPutPortComboEnableChanged();

    void showInputBluetoothModeChanged();

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

    QStringList m_outPutPortCombo;
    int m_outPutPortComboIndex;
    bool m_outPutPortComboEnable;

    QStringList m_inPutPortCombo;
    int m_inPutPortComboIndex;
    bool m_inPutPortComboEnable;

    SoundEffectsModel* m_soundEffectsModel;
    SoundDeviceModel* m_soundInputDeviceModel;
    SoundDeviceModel* m_soundOutputDeviceModel;
    AudioServerModel* m_audioServerModel;

    int m_inPutPortCount;
    int m_outPutCount;

    bool m_audioMono;
    bool m_showBluetoothMode;
    bool m_showInputBluetoothMode;
};

#endif // DCC_SOUND_SOUNDMODEL_H
