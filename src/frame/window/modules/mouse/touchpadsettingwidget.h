// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TOUCHPADSETTINGWIDGET_H
#define TOUCHPADSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
class ContentWidget;
namespace mouse {
class MouseModel;
class MouseWorker;
class DouTestWidget;
class PalmDetectSetting;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class TouchPadSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TouchPadSettingWidget(QWidget *parent = nullptr);
    void setModel(dcc::mouse::MouseModel *const model);

Q_SIGNALS:
    void requestSetTouchpadMotionAcceleration(const int value);
    void requestSetTapClick(const bool state);
    void requestSetTouchNaturalScroll(const bool state);
    void requestDetectState(bool enable);
    void requestContact(int value);
    void requestPressure(int value);
    void requestSetTouchpadEnable(const bool state);

private Q_SLOTS:
    void onTouchMoveSpeedChanged(int speed);

private:
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::mouse::PalmDetectSetting *m_palmDetectSetting;
    dcc::widgets::SwitchWidget *m_touchPadEnable;
    dcc::widgets::TitledSliderItem *m_touchMoveSlider;
    dcc::widgets::SwitchWidget *m_touchClickStn;
    dcc::widgets::SwitchWidget *m_touchNaturalScroll;
    QVBoxLayout *m_contentLayout;
    dcc::ContentWidget *m_contentWidget;
};
}
}
#endif // TOUCHPADSETTINGWIDGET_H
