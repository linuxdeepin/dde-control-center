/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef EDITLINESWITCHBUTTON_H
#define EDITLINESWITCHBUTTON_H

#include "networkbaseeditline.h"

class EditLineSwitchButton : public NetworkBaseEditLine
{
public:
    explicit EditLineSwitchButton(const QString &section, const QString &key,
                                 DBusConnectionSession *dbus,
                                 QWidget *parent = 0);
};

#endif // EDITLINESWITCHBUTTON_H
