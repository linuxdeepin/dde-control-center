/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
