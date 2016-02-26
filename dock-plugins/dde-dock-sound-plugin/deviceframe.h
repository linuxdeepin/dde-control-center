/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DEVICEFRAME_H
#define DEVICEFRAME_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "dbus/dbusaudio.h"
#include "deviceiconslider.h"

class DeviceFrame : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceFrame(QWidget *parent = 0);

signals:
    void heightChanged(int height);

private slots:
    void initTitle();
    void initDevice();
    void reloadDevice();

private:
    DBusAudio *m_audio;
    DeviceIconSlider *m_iconSlider = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
};

#endif // DEVICEFRAME_H
