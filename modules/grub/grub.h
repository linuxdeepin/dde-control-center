/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GRUB_H
#define GRUB_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"

class QFrame;
class GrubWidget;
class Grub: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "grub.json")
    Q_INTERFACES(ModuleInterface)

public:
    Grub();
    ~Grub() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    QFrame *m_grubWidget;
};

#endif // GRUB

