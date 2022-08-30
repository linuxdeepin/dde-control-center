// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TRACKPOINTSETTINGWIDGET_H
#define TRACKPOINTSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace mouse {
class MouseModel;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class TrackPointSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit TrackPointSettingWidget(QWidget *parent = nullptr);
    void setModel(dcc::mouse::MouseModel *const model);

Q_SIGNALS:
    void requestSetTrackPointMotionAcceleration(const int value);

private Q_SLOTS:
    void onRedPointMoveSpeedChanged(int speed);

private:
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::widgets::SettingsGroup *m_trackPointSettingsGrp;
    dcc::widgets::TitledSliderItem *m_trackMoveSlider;
    QVBoxLayout *m_contentLayout;
};
}
}
#endif // TRACKPOINTSETTINGWIDGET_H
