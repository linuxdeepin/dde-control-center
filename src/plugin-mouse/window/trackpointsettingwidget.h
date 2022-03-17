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
#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class MouseModel;
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;

class TrackPointSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrackPointSettingWidget(QWidget *parent = nullptr);
    void setModel(MouseModel *const model);

Q_SIGNALS:
    void requestSetTrackPointMotionAcceleration(const int value);

private Q_SLOTS:
    void onRedPointMoveSpeedChanged(int speed);

private:
    MouseModel *m_mouseModel;
    SettingsGroup *m_trackPointSettingsGrp;
    TitledSliderItem *m_trackMoveSlider;
    QVBoxLayout *m_contentLayout;
};
}
#endif // TRACKPOINTSETTINGWIDGET_H
