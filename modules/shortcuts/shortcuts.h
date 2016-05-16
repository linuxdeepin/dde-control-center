/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"

class QFrame;
class MainWidget;

class Shortcuts: public QObject
{
    Q_OBJECT
public:
    Shortcuts(QObject *parent = NULL);
    QFrame *getContent();
private:
    QFrame *m_mainWidget = NULL;
};

class ShortcutsModule: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "shortcuts.json")
    Q_INTERFACES(ModuleInterface)

public:
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    Shortcuts *m_shortcuts = NULL;
};

#endif // SHORTCUTS_H

