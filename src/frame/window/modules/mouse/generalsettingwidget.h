// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GENERALSETTINGWIDGET_H
#define GENERALSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace mouse {
class MouseModel;
class MouseWorker;
class DouTestWidget;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class GeneralSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit GeneralSettingWidget(QWidget *parent = nullptr);
    ~GeneralSettingWidget();

    void setModel(dcc::mouse::MouseModel *const model);
Q_SIGNALS:
    void requestSetLeftHand(const bool state);
    void requestSetDisTyping(const bool state);
    void requestScrollSpeed(const int speed);
    void requestSetDouClick(const int value);
private:
    void onDoubleClickSpeedChanged(int speed);
    void onScrollSpeedChanged(uint speed);
private:
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::widgets::SettingsGroup *m_generalSettingsGrp;

    dcc::widgets::SwitchWidget *m_leftHand;
    dcc::widgets::SwitchWidget *m_disInTyping;
    dcc::widgets::TitledSliderItem *m_doubleSlider;
    dcc::widgets::TitledSliderItem *m_scrollSpeedSlider;
    dcc::mouse::DouTestWidget *m_doubleTest;
    QVBoxLayout *m_contentLayout;
};
}
}
#endif // GENERALSETTINGWIDGET_H
