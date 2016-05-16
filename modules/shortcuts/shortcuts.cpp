/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QLabel>

#include "mainwidget.h"
#include "shortcuts.h"

QFrame *ShortcutsModule::getContent()
{
    qDebug() << "new Shortcuts begin";
    if (NULL == m_shortcuts) {
        m_shortcuts = new Shortcuts(this);
    }
    qDebug() << "new Shortcuts end";
    return m_shortcuts->getContent();
}

Shortcuts::Shortcuts(QObject *parent): QObject(parent)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Keyboard Shortcuts"));
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);
    m_mainWidget = new MainWidget();
}

QFrame *Shortcuts::getContent()
{
    return m_mainWidget;
}
