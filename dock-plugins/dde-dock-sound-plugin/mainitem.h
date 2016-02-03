/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MAINITEM_H
#define MAINITEM_H

#include <QLabel>
#include <QWheelEvent>
#include <QDebug>
#include "dbus/dbusaudio.h"
#include "dbus/dbusaudiosink.h"
#include <dde-dock/dockconstants.h>
#include "soundicon.h"

class MainItem : public QLabel
{
    Q_OBJECT
public:
    MainItem(QWidget *parent = 0);
    void setDockMode(Dock::DockMode mode);

protected:
    void wheelEvent(QWheelEvent *);

private:
    void initDefaultSink();
    void updateIcon();

private:
    DBusAudio *m_audio;
    DBusAudioSink *m_defaultSkin = NULL;
    Dock::DockMode m_dockMode = Dock::FashionMode;
    const double WHEEL_STEP = 0.1;
};

#endif // MAINITEM_H
