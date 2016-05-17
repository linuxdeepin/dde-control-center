/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QtPlugin>
#include <QFrame>

#include "interfaces.h"



#include "interfaces.h"


namespace Plugin
{
namespace Sound
{

class SoundView;
class SoundControl;

class SoundModule: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "sound.json")
    Q_INTERFACES(ModuleInterface)

public:
    SoundModule();
    ~SoundModule() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    SoundView       *m_view     = nullptr;
    SoundControl    *m_control  = nullptr;
};


}
}

#endif
