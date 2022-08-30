// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SPEAKERPAGE_H_V20
#define SPEAKERPAGE_H_V20

#include "interface/namespace.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/comboxwidget.h"
#include "modules/sound/soundmodel.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QTimer>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

#define ICON_SIZE 24
namespace dcc {

namespace sound {
class SoundModel;
}

namespace widgets {
class TitledSliderItem;
class SettingsGroup;
class SwitchWidget;
class ComboxWidget;
class Port;
}
}

namespace DCC_NAMESPACE {

namespace sound {

/**
 * 该界面类对应 控制中心-声音模块-扬声器 界面
 * 界面包含一个 switch 控件用于 打开/关闭扬声器
 * 包含两个 slider 控件用于 调节输出音量和左/右声道平衡
 */
class SpeakerPage : public QWidget
{
    Q_OBJECT
public:
    SpeakerPage(QWidget *parent = nullptr);
    ~SpeakerPage();
    void showDevice();
    void setDeviceVisible(bool visible);
    void setBlueModeVisible(bool visible);

public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    //请求改变输出音量 0-1.5
    void requestSetSpeakerVolume(double val);
    //请求改变左右平衡 0-1.5
    void requestSetSpeakerBalance(double val);
    //请求改变音量增强
    void requestIncreaseVolume(bool value);
    void requestSetPort(const dcc::sound::Port *);
    //请求静音切换,flag为false时请求直接取消静音
    void requestMute(bool flag = true);
    //请求切换蓝牙耳机模式
    void requstBluetoothMode(QString blueMode);

private Q_SLOTS:
    void removePort(const QString &portId, const uint &cardId, const dcc::sound::Port::Direction &direction);
    void addPort(const dcc::sound::Port *port);
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
    void refreshActivePortShow(const dcc::sound::Port *port);
    bool hasVirtualSink();

private:
    //model类， 为后端数据来源及数据变化信号来源
    dcc::sound::SoundModel *m_model{nullptr};
    dcc::widgets::SettingsGroup *m_outputSoundsGrp;
    //输入列表的下拉框列表
    dcc::widgets::ComboxWidget *m_outputSoundCbx;
    //蓝牙耳机下拉框
    dcc::widgets::ComboxWidget *m_blueSoundCbx;

    //界面的主layout
    QVBoxLayout *m_layout{nullptr};
    dcc::widgets::TitledSliderItem *m_outputSlider;
    dcc::widgets::DCCSlider *m_speakSlider;
    QWidget* m_vbWidget;
    dcc::widgets::TitledSliderItem *m_balanceSlider;
    QStandardItemModel *m_outputModel{nullptr};
    //当前选中的音频 即activeport
    const dcc::sound::Port *m_currentPort{nullptr};
    int m_waitTimerValue;
    int m_lastRmPortIndex;
    //左/右平衡音界面是否显示
    bool m_balance;
    dcc::sound::SoundLabel *m_volumeBtn;
    bool m_mute;
    //启用端口但未设置为默认端口判断
    bool m_enablePort;
    // 蓝牙模式信息
    QStringList m_bluetoothModeOpts;
    // 确保第一次点击没有延时
    bool m_fristChangePort;
    bool m_fristStatusChangePort;
    // 名称
    QString m_speakerName;

    /**
     * @brief m_waitStatusChangeTimer 端口切换等待延时
     */
    QTimer *m_waitStatusChangeTimer;
};

}
}

#endif // SPEAKERPAGE_H_V20
