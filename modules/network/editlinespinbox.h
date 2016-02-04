/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef EDITLINESPINBOX_H
#define EDITLINESPINBOX_H

#include "networkbaseeditline.h"

class EditLineSpinBox : public NetworkBaseEditLine
{
    Q_OBJECT
public:
    explicit EditLineSpinBox(const QString &section, const QString &key,
                             DBusConnectionSession *dbus,
                             int minValue = 0, int maxValue = 0, QWidget *parent = 0);
};

#endif // EDITLINESPINBOX_H
