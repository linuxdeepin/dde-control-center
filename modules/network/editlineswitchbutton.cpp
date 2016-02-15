/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dswitchbutton.h>

#include "editlineswitchbutton.h"
#include "networkglobal.h"

DWIDGET_USE_NAMESPACE

EditLineSwitchButton::EditLineSwitchButton(const QString &section, const QString &key,
                                           DBusConnectionSession *dbus,
                                           QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, parent)
{
    DSwitchButton *switch_button = new DSwitchButton;

    auto updateSwitchButtonCheckeState = [switch_button, this] {
        SIGNAL_BLOCKE(switch_button);

        switch_button->setChecked(cacheValue().toBool());
    };

    connect(this, &NetworkBaseEditLine::widgetShown, this, updateSwitchButtonCheckeState);
    connect(this, &NetworkBaseEditLine::cacheValueChanged, this, updateSwitchButtonCheckeState);
    connect(switch_button, &DSwitchButton::checkedChanged, this, [this](bool checked) {
        setDBusKey(checked);
    });

    setRightWidget(switch_button);

    if(cacheValue().isNull())
        updateSwitchButtonCheckeState();
}
