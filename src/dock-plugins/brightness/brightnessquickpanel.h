// Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "brightnesscontroller.h"
#include "monitor.h"

#include <DBlurEffectWidget>
#include <QWidget>
#include <QPointer>

class QDBusMessage;
class SliderContainer;
class QLabel;

DWIDGET_USE_NAMESPACE

class BrightnessQuickPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BrightnessQuickPanel(QWidget *parent = nullptr);
    ~BrightnessQuickPanel() override;

Q_SIGNALS:
    void requestShowApplet();

protected:
    void initUi();
    void initConnection();

private Q_SLOTS:
    void refreshWidget();
    void UpdateDisplayStatus();

private:
    SliderContainer *m_sliderContainer;
    QPointer<Monitor> m_currentMonitor;
};
