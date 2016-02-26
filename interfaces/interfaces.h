/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef INTERFACES_H
#define INTERFACES_H

class QFrame;
class ControlCenterProxyInterface;
class ModuleInterface
{
public:
    virtual ~ModuleInterface() {}
    virtual QFrame *getContent() = 0;

public:
    // plugin can remove there own data on this function(remove popup window for example).
    // TODO
    // preUnload should be executed before every unload operations.
    virtual void preUnload() {}
    virtual inline void setProxy(ControlCenterProxyInterface *proxy) {m_controlCenterProxy = proxy;}

protected:
    ControlCenterProxyInterface *m_controlCenterProxy = nullptr;
};


QT_BEGIN_NAMESPACE

#define ModuleInterface_iid "org.deepin.ControlCenter.ModuleInterface"

Q_DECLARE_INTERFACE(ModuleInterface, ModuleInterface_iid)
QT_END_NAMESPACE

#endif
