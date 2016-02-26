/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPFRAME_H
#define APPFRAME_H

#include <QMap>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "appiconslider.h"
#include "dbus/dbusaudio.h"

class AppFrame : public QWidget
{
    Q_OBJECT
public:
    explicit AppFrame(QWidget *parent = 0);
    QSize getContentSize();
    void updateSize();

signals:
    void appCountChanged(int count);

private:
    void initTitle();
    void initAppList();
    void updateAppList();

private:
    DBusAudio *m_audio = NULL;
    QMap<QString, AppIconSlider *> m_appsMap;
    QVBoxLayout *m_mainLayout = NULL;
};

#endif // APPFRAME_H
