/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TITLEDSETTINGSITEM_H
#define TITLEDSETTINGSITEM_H

#include "settingsitem.h"

#include <QLabel>

using namespace dcc;

class TitledSettingsItem : public SettingsItem
{
public:
    explicit TitledSettingsItem(QFrame * parent = 0);

    QString title() const;
    void setTitle(const QString &title);

    QWidget *widget();
    void setWidget(QWidget *widget);

private:
    QLabel *m_title;
    QWidget *m_widget;
};

#endif // TITLEDSETTINGSITEM_H
