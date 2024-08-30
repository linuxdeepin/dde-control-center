//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SPEAKERPAGE_H_V20
#define SPEAKERPAGE_H_V20

#include "interface/namespace.h"
#include "soundmodel.h"

#include <QWidget>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QTimer;
class QStandardItemModel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class TitledSliderItem;
class SettingsGroup;
class DCCSlider;
class ComboxWidget;
}


#define ICON_SIZE 24

/**
 * 该界面类对应 控制中心-声音模块-扬声器 界面
 * 界面包含一个 switch 控件用于 打开/关闭扬声器
 * 包含两个 slider 控件用于 调节输出音量和左/右声道平衡
 */

class SpeakerPage : public QWidget
{
    Q_OBJECT
public:
    explicit SpeakerPage(QWidget *parent = nullptr);
    ~SpeakerPage();
    void showDevice();
    void setDeviceVisible(bool visible);
    void setBlueModeVisible(bool visible);

public:
    void setModel(SoundModel *model);

Q_SIGNALS:
    //请求改变输出音量 0-1.5
    void requestSetSpeakerVolume(double val);
    //请求改变左右平衡 0-1.5
    void requestSetSpeakerBalance(double val);
    //请求改变音量增强
    void requestIncreaseVolume(bool value);
    void requestSetPort(const Port *);
    //请求静音切换,flag为false时请求直接取消静音
    void requestMute(bool flag = true);
    //请求切换蓝牙耳机模式
    void requstBluetoothMode(QString blueMode);

private Q_SLOTS:
    void removePort(const QString &portId, const uint &cardId, const Port::Direction &direction);
    void addPort(const Port *port);
    /**
     * @brief changeComboxIndex  处理用户点击下拉框， 选择对应port端口操作， 需要对 setport 进行延时
     * @param idx
     */
    void changeComboxIndex(const int idx);
    /**
     * @brief changeComboxStatus  仅处理port插拔时的延时置灰操作
     * @param idex
     */
    void changeComboxStatus();
    void clickLeftButton();
    void changeBluetoothMode(const int idx);

private:
    void initSlider();
    void initCombox();
    void refreshIcon();
    void showWaitSoundPortStatus(bool showStatus);
    /**
     * @brief refreshActivePortShow 最终确认显示的端口下拉框 保证蓝牙下拉框与之匹配
     */
    void refreshActivePortShow(const Port *port);

private:
    //model类， 为后端数据来源及数据变化信号来源
    SoundModel *m_model{nullptr};
    DCC_NAMESPACE::SettingsGroup *m_outputSoundsGrp;
    //输入列表的下拉框列表
    DCC_NAMESPACE::ComboxWidget *m_outputSoundCbx;
    //蓝牙耳机下拉框
    DCC_NAMESPACE::ComboxWidget *m_blueSoundCbx;

    //界面的主layout
    QVBoxLayout *m_layout{nullptr};
    DCC_NAMESPACE::TitledSliderItem *m_outputSlider;
    DCC_NAMESPACE::DCCSlider *m_speakSlider;
    QWidget* m_vbWidget;
    DCC_NAMESPACE::TitledSliderItem *m_balanceSlider;
    QStandardItemModel *m_outputModel{nullptr};
    //当前选中的音频 即activeport
    const Port *m_currentPort{nullptr};
    int m_waitTimerValue;
    int m_lastRmPortIndex;
    //左/右平衡音界面是否显示
    bool m_balance;
    SoundLabel *m_volumeBtn;
    //启用端口但未设置为默认端口判断
    bool m_enablePort;
    // 蓝牙模式信息
    QStringList m_bluetoothModeOpts;
    // 确保第一次点击没有延时
    bool m_fristChangePort;
    bool m_fristStatusChangePort;

    /**
     * @brief m_waitStatusChangeTimer 端口切换等待延时
     */
    QTimer *m_waitStatusChangeTimer;
};

#endif // SPEAKERPAGE_H_V20
