/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SYSTEMPROXYWIDGET_H
#define SYSTEMPROXYWIDGET_H

#include <dboxwidget.h>

#include "dbus/dbusnetwork.h"

DWIDGET_USE_NAMESPACE

class SystemProxyWidget : public DVBoxWidget
{
    Q_OBJECT

public:
    explicit SystemProxyWidget(DBusNetwork *dbus, QWidget *parent = 0);

private:
    int m_currentIndex = 0;
};

#endif // SYSTEMPROXYWIDGET_H
