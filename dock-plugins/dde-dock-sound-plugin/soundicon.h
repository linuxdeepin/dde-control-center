/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDICON_H
#define SOUNDICON_H

#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <dde-dock/dockconstants.h>

class SoundIcon : public QObject
{
    Q_OBJECT
public:
    SoundIcon();

    static QPixmap getDefaultSinkDockIcon(Dock::DockMode mode, int volume, bool isMute = false);
    static QPixmap getDefaultSinkIcon(Dock::DockMode mode, int volume, bool isMute = false);
    static QPixmap getDefaultSinkIcon(int size, int volume, bool isMute = false);
    static QPixmap getAppSinkIcon(Dock::DockMode mode,const QString &iconName);
    static QPixmap getAppSinkIcon(int size, const QString &iconName);

};

#endif // SOUNDICON_H
