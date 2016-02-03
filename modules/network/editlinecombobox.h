/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef EDITLINECOMBOBOX_H
#define EDITLINECOMBOBOX_H

#include <libdui/dcombobox.h>

#include "networkbaseeditline.h"

DUI_USE_NAMESPACE

class EditLineComboBox : public NetworkBaseEditLine
{
public:
    explicit EditLineComboBox(const QString &section, const QString &key,
                              DBusConnectionSession *dbus, const QString &title,
                              bool editable = false, QWidget *parent = 0);

    DComboBox *comboBox() const;

private:
    DComboBox *m_comboBox;
};

#endif // EDITLINECOMBOBOX_H
