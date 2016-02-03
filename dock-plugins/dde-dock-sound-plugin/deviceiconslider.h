/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DEVICEICONSLIDER_H
#define DEVICEICONSLIDER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSlider>
#include <QDebug>
#include "iconlabel.h"
#include "soundicon.h"
#include "volumeslider.h"
#include "dbus/dbusaudiosink.h"
#include <dde-dock/dockconstants.h>

class DeviceIconSlider : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceIconSlider(const QString &interfacePath, QWidget *parent = 0);

    void changeMode(Dock::DockMode mode);
signals:

public slots:
    void volumeUpdate();

private:
    void initSink(const QString &path);
    void initWidget();

private:
    Dock::DockMode m_dockMode = Dock::EfficientMode;
    VolumeSlider *m_iSlider = NULL;
    IconLabel *m_iLabel = NULL;
    QTimer *m_callDBusTimer = NULL;
    DBusAudioSink *m_das = NULL;
    const int ICON_SIZE = 24;
};

#endif // DEVICEICONSLIDER_H
