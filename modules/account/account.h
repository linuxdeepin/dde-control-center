/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QFrame;
class AccountMainWidget;

class Account: public QObject
{
    Q_OBJECT

public:
    Account();
    ~Account();
    QFrame *getContent();
    void preUnload();

    AccountMainWidget *mainWidget = NULL;
};

class AccountModule: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "account.json")
    Q_INTERFACES(ModuleInterface)

public:
    AccountModule();
    ~AccountModule() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private slots:
    void preUnload() Q_DECL_OVERRIDE;

private:
    Account *account = NULL;
};



#endif
