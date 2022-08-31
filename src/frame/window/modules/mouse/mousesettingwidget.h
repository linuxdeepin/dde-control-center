// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MOUSESETTINGWIDGET_H
#define MOUSESETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace mouse {
class MouseModel;
class MouseWorker;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class MouseSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit MouseSettingWidget(QWidget *parent = nullptr);
    ~MouseSettingWidget();

    void setModel(dcc::mouse::MouseModel *const model);

Q_SIGNALS:
    void requestSetMouseMotionAcceleration(const int value);
    void requestSetAccelProfile(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetMouseNaturalScroll(const bool state);

private Q_SLOTS:
    void onMouseMoveSpeedChanged(int speed);

private:
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::widgets::SettingsGroup *m_mouseSettingsGrp;
    dcc::widgets::TitledSliderItem *m_mouseMoveSlider;
    dcc::widgets::SwitchWidget *m_adaptiveAccelProfile;
    dcc::widgets::SwitchWidget *m_disTchStn;
    dcc::widgets::SwitchWidget *m_mouseNaturalScroll;
    QVBoxLayout *m_contentLayout;
};
}
}
#endif // MOUSESETTINGWIDGET_H
