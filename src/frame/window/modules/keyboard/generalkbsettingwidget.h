// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GENERALKBSETTINGWIDGET_H
#define GENERALKBSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include <QMouseEvent>
#include <DLineEdit>

namespace dcc {
namespace keyboard {
class KeyboardModel;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class DCCSlider;
class GLineEdit;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {

#define GSETTINGS_NUMLOCK_ENABLE "keyboardGeneralNumlockEnable"
#define GSETTINGS_CAPPSLOCK_ENABLE "keyboardGeneralCapslockEnable"

class GeneralKBSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit GeneralKBSettingWidget(dcc::keyboard::KeyboardModel *model, QWidget *parent = nullptr);
Q_SIGNALS:
    void requestKBDelayChanged(const int value);
    void requestKBSpeedChanged(const int value);
    void requestNumLockChanged(const bool state);
    void requestCapsLockChanged(const bool state);
private Q_SLOTS:
    void setDelayValue(uint value);
    void setSpeedValue(uint value);
private:
    dcc::widgets::DCCSlider *m_delaySlider;
    dcc::widgets::DCCSlider *m_speedSlider;
    dcc::widgets::SwitchWidget *m_upper;
    dcc::widgets::SwitchWidget *m_numLock;
    dcc::widgets::SettingsGroup *m_generalSettingsGrp;
    dcc::keyboard::KeyboardModel *m_model;
    QVBoxLayout *m_contentLayout;
    DLineEdit *m_testArea;
};
}
}
#endif // GENERALKBSETTINGWIDGET_H
