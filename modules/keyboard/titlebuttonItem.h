/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TITLEBUTTONITEM_H
#define TITLEBUTTONITEM_H

#include "settingsitem.h"

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

using namespace dcc::widgets;

class TitleButtonItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit TitleButtonItem(QFrame* parent = 0);
    void setTitle(const QString& title);
    void setValue(const QString& value);

signals:
    void click();

private:
    QLabel *m_title;
    QPushButton *m_button;
};

#endif // TITLEBUTTONITEM_H
