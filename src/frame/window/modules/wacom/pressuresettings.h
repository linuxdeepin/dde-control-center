/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#pragma once

#include "interface/namespace.h"

#include "widgets/translucentframe.h"

#include <DSlider>

#include <QObject>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {
class SettingsGroup;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace wacom {
class WacomModel;

class PressureSettings : public dcc::widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit PressureSettings(QWidget *parent = nullptr);
    void setModel(WacomModel *model);

Q_SIGNALS:
    void requestSetPressureValue(const int &value);

public Q_SLOTS:
    void setPressureValue(const uint &value);

private:
    QVBoxLayout *m_mainLayout;
    dcc::widgets::SettingsGroup *m_mainGroup;
    WacomModel *m_model;
    dcc::widgets::TitledSliderItem *m_pressureSlider;
    DSlider *m_preSlider;
};

}
}
