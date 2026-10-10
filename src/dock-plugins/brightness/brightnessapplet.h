// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BRIGHTNESSAPPLET_H
#define BRIGHTNESSAPPLET_H

#include "jumpsettingbutton.h"
#include "monitor.h"
#include "slidercontainer.h"

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

class BrightnessApplet : public QWidget
{
    Q_OBJECT
public:
    explicit BrightnessApplet(QWidget* parent = nullptr);
    ~BrightnessApplet();

    void setAppletMinHeight(int minHeight);
    void onContainerChanged(int container);

Q_SIGNALS:
    void requestHideApplet();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void initUI();
    void initConnections();
    void resizeScrollArea();
    void addMonitor(Monitor *monitor);
    void updateMonitors();

private:
    QWidget *m_titleWidget;
    QScrollArea *m_scrollArea;
    QVBoxLayout *m_monitorsLayout;
    JumpSettingButton *m_jumpSettingButton;
    int m_minHeight;
};

#endif
