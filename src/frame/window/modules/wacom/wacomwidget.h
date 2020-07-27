/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wp <wangpeng_cm@deepin.com>
 *
 * Maintainer: wp <wangpeng_cm@deepin.com>
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

#ifndef WACOMWIDGET_H_V20
#define WACOMWIDGET_H_V20

#include "interface/namespace.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>

namespace dcc {
namespace widgets {
class SettingsItem;
}
}

namespace DCC_NAMESPACE {
namespace wacom {

class PressureSettings;
class WacomModel;

class WacomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WacomWidget(QWidget *parent = nullptr);
    void setModel(WacomModel *model);

Q_SIGNALS:
    void requestSetPressureValue(const int value);
    void modeChanged(const bool curMode);

private:
    void initWidget();

private Q_SLOTS:
    void onCursorModeChanged(const bool curMode);

private:
    PressureSettings *m_sensitivity;
    QVBoxLayout *m_centralLayout;
    QHBoxLayout *m_modeLayout;
    QLabel *m_modeTitle;
    QComboBox *m_modeComboBox;
    dcc::widgets::SettingsItem *m_modeSetting;
};
}
}


#endif // WACOMWIDGET_H_V20
