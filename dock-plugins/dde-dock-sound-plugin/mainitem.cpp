/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "mainitem.h"

MainItem::MainItem(QWidget *parent) : QLabel(parent)
{
    m_audio = new DBusAudio(this);
    initDefaultSink();
    //data will change with default-sink-change
    connect(m_audio, &DBusAudio::DefaultSinkChanged, this, [=] {
        disconnect(m_defaultSkin,&DBusAudioSink::VolumeChanged,this,&MainItem::updateIcon);
        disconnect(m_defaultSkin,&DBusAudioSink::MuteChanged,this,&MainItem::updateIcon);
        m_defaultSkin->deleteLater();
        //audio-backend's data is always delayed loaging
        QTimer *t = new QTimer(this);
        t->setSingleShot(true);
        connect(t, &QTimer::timeout, this, [=] {
            initDefaultSink();
            updateIcon();
            sender()->deleteLater();
        });
        t->start(1000);
    });
}

void MainItem::setDockMode(Dock::DockMode mode)
{
    m_dockMode = mode;

    updateIcon();
}

void MainItem::wheelEvent(QWheelEvent * event)
{
    if (m_defaultSkin && m_defaultSkin->isValid())
    {
        m_defaultSkin->SetMute(false);

        double tmp = m_defaultSkin->volume();
        if (event->angleDelta().y() > 0)
            m_defaultSkin->SetVolume(tmp > 0.9 ? 1.0 : tmp + WHEEL_STEP,m_defaultSkin->mute());
        else
            m_defaultSkin->SetVolume(tmp < 0.1 ? 0 : tmp - WHEEL_STEP,m_defaultSkin->mute());

        event->accept();

        updateIcon();
    }
}

void MainItem::initDefaultSink()
{
    QString path = QDBusObjectPath(m_audio->GetDefaultSink().value()).path();
    m_defaultSkin = new DBusAudioSink(path,this);
    connect(m_defaultSkin,&DBusAudioSink::VolumeChanged,this,&MainItem::updateIcon);
    connect(m_defaultSkin,&DBusAudioSink::MuteChanged,this,&MainItem::updateIcon);
}

void MainItem::updateIcon()
{
    if (!m_defaultSkin)
        setPixmap(SoundIcon::getDefaultSinkDockIcon(m_dockMode,0,true));
    else
        setPixmap(SoundIcon::getDefaultSinkDockIcon(m_dockMode,int(m_defaultSkin->volume() * 100),m_defaultSkin->mute()));
}
