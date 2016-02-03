/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPICONSLIDER_H
#define APPICONSLIDER_H

#include <QWidget>
#include <QSlider>
#include <QHBoxLayout>
#include "soundicon.h"
#include "iconlabel.h"
#include "volumeslider.h"
#include "dbus/dbusaudiosinkinput.h"

class AppIconSlider : public QWidget
{
    Q_OBJECT
public:
    explicit AppIconSlider(const QString &interfacePath, QWidget *parent = 0);

    QString interfacePath() const;
signals:

public slots:
    void volumeUpdate();
    bool isValid();

private:
    void initSinkInput(const QString &path);
    void initWidget();

private:
    QString m_interfacePath = "/";
    DBusAudioSinkInput * m_dasi = NULL;
    VolumeSlider *m_iSlider = NULL;
    IconLabel *m_iLabel = NULL;
    QLabel *m_muteIcon = NULL;
    QTimer *m_callDBusTimer = NULL;

    const int ICON_SIZE = 26;
};

#endif // APPICONSLIDER_H
