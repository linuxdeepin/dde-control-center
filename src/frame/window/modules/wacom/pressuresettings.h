// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
