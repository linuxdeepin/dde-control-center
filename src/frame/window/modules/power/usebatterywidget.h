// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <DListView>

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class TitledSliderItem;
class SwitchWidget;
class ComboxWidget;
}

namespace power {
class PowerModel;
class PowerWorker;
}
}

namespace DCC_NAMESPACE {
namespace power {

typedef QMap<int, QString> ActionList;

class UseBatteryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UseBatteryWidget(dcc::power::PowerModel *model, QWidget *parent = nullptr, dcc::power::PowerWorker *worker = nullptr);
    virtual ~UseBatteryWidget();

    void setModel(const dcc::power::PowerModel *model);

private:
    QString delayToLiteralString(const int delay) const;
    void setComboxOption(dcc::widgets::ComboxWidget *combox, ActionList options);

private:
    QVBoxLayout *m_layout;
    dcc::widgets::TitledSliderItem *m_monitorSleepOnBattery;
    dcc::widgets::TitledSliderItem *m_computerSleepOnBattery;
    dcc::widgets::TitledSliderItem *m_autoLockScreen;

    dcc::widgets::ComboxWidget *m_cmbPowerBtn; // 电源按钮
    dcc::widgets::ComboxWidget *m_cmbCloseLid; // 合盖

    DTK_WIDGET_NAMESPACE::DListView *m_lowPowerListView = nullptr;
    QStandardItemModel *m_lowPowerModel = nullptr;
    dcc::widgets::SwitchWidget *m_swBatteryHint;
    dcc::widgets::TitledSliderItem *m_sldLowBatteryHint;
    QMap<int, int> m_sldLowBatteryMap;

    dcc::widgets::TitledSliderItem *m_sldAutoSuspend;
    dcc::power::PowerModel *m_model;
    dcc::power::PowerWorker *m_work;

Q_SIGNALS:
    void requestSetScreenBlackDelayOnBattery(const int delay) const;
    void requestSetSleepDelayOnBattery(const int delay) const;
    void requestSetAutoLockScreenOnBattery(const int delay) const;
    void requestSetLowPowerNotifyEnable(const bool bState) const;     // 低电量通知
    void requestSetBatteryPressPowerBtnAction(const int reply) const; // 按下电源
    void requestSetBatteryLidClosedAction(const int reply) const;     // 合上盖子
    void requestSetLowPowerNotifyThreshold(const int dValue);         // 低电量通知阈值
    void requestSetLowPowerAutoSleepThreshold(const int dValue);      // 进入待机模式阈值

public Q_SLOTS:
    void setScreenBlackDelayOnBattery(const int delay);
    void setSleepDelayOnBattery(const int delay);
    void setAutoLockScreenOnBattery(const int delay);
    void onLowPowerNotifyThreshold(const int value);
    void onLowPowerAutoSleepThreshold(const int value);
    void setCloseLid(const dcc::power::PowerModel *model, int option);
    void setPowerBtn(const dcc::power::PowerModel *model, int option);
    void updatePowerButtonActionList();
};

}// namespace datetime
}// namespace DCC_NAMESPACE
