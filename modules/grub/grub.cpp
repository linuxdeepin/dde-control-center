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

#include "grub.h"
#include "grubwidget.h"

Grub::Grub():
    m_grubWidget(new GrubWidget())
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Boot Menu"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);
}

Grub::~Grub()
{
    qDebug() << "~Grub()";
    m_grubWidget->deleteLater();
}

QFrame *Grub::getContent()
{
    return m_grubWidget;
}

