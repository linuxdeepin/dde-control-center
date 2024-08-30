//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
