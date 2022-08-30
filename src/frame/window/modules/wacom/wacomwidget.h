// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
