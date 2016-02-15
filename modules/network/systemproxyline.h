/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SYSTEMPROXYLINE_H
#define SYSTEMPROXYLINE_H

#include <dheaderline.h>

#include "networkglobal.h"
#include "dbus/dbusnetwork.h"

DWIDGET_USE_NAMESPACE

class SystemProxyLine : public DHeaderLine
{
    Q_OBJECT

public:
    explicit SystemProxyLine(const QString &title, const QString &proxyType,
                             DBusNetwork *dbus, QWidget *parent = 0);
};

#endif // SYSTEMPROXYLINE_H
