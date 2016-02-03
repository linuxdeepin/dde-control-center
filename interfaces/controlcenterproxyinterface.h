/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONTROLCENTERPROXYINTERFACE_H
#define CONTROLCENTERPROXYINTERFACE_H

class QObject;
class ControlCenterProxyInterface
{
public:
    ControlCenterProxyInterface() {}

    virtual void hide(bool imme = false) = 0;
    virtual void setAutoHide(bool autoHide) = 0;
    virtual bool isVisible() const = 0;
    virtual QObject *dccObject() const = 0;
};

#endif // CONTROLCENTERPROXYINTERFACE_H
