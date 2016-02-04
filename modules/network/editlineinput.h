/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef EDITLINEINPUT_H
#define EDITLINEINPUT_H

#include "networkbaseeditline.h"
#include "networkglobal.h"

class EditLineInput : public NetworkBaseEditLine
{
public:
    enum EditLineInputType {
        Normal,
        Password,
        FileChooser,
        Ipv4Input
    };

    explicit EditLineInput(const QString &section, const QString &key,
                           DBusConnectionSession *dbus,
                           EditLineInputType type = Normal,
                           bool alwaysUpdate = false, QWidget *parent = 0);
};

#endif // EDITLINEINPUT_H
