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
    connect(m_audio, &DBusAudio::DefaultSinkChanged, this, &MainItem::reloadDefaultSink);
}

void MainItem::setDockMode(Dock::DockMode mode)
{
    m_dockMode = mode;

    updateIcon();
}

void MainItem::wheelEvent(QWheelEvent *event)
{
    if (m_defaultSkink && m_defaultSkink->isValid()) {
        m_defaultSkink->SetMute(false);

        double tmp = m_defaultSkink->volume();
        if (event->angleDelta().y() > 0) {
            m_defaultSkink->SetVolume(tmp > 0.9 ? 1.0 : tmp + WHEEL_STEP, m_defaultSkink->mute());
        } else {
            m_defaultSkink->SetVolume(tmp < 0.1 ? 0 : tmp - WHEEL_STEP, m_defaultSkink->mute());
        }

        event->accept();

        updateIcon();
    }
}

void MainItem::initDefaultSink()
{
    QString path = QDBusObjectPath(m_audio->GetDefaultSink().value()).path();
    m_defaultSkink = new DBusAudioSink(path, this);
    connect(m_defaultSkink, &DBusAudioSink::VolumeChanged, this, &MainItem::updateIcon);
    connect(m_defaultSkink, &DBusAudioSink::MuteChanged, this, &MainItem::updateIcon);
}

void MainItem::updateIcon()
{
    if (!m_defaultSkink) {
        setPixmap(SoundIcon::getDefaultSinkDockIcon(m_dockMode, 0, true));
    } else {
        setPixmap(SoundIcon::getDefaultSinkDockIcon(m_dockMode, int(m_defaultSkink->volume() * 100), m_defaultSkink->mute()));
    }
}

void MainItem::reloadDefaultSink()
{
    if (!m_defaultSkink)
        return;

    // clear old data.
    disconnect(m_defaultSkink, &DBusAudioSink::VolumeChanged, this, &MainItem::updateIcon);
    disconnect(m_defaultSkink, &DBusAudioSink::MuteChanged, this, &MainItem::updateIcon);
    m_defaultSkink->deleteLater();
    m_defaultSkink = nullptr;

    //audio-backend's data is always delayed loaging
    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    connect(t, &QTimer::timeout, this, [ = ] {
        initDefaultSink();
        updateIcon();
        sender()->deleteLater();
    });
    t->start(1000);
}
