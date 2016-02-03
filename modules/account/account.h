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
#include "ui/accountmainwidget.h"

class QLabel;
class QFrame;
class Account: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "account.json")
    Q_INTERFACES(ModuleInterface)

public:
    Account();
    ~Account() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private slots:
    void preUnload() Q_DECL_OVERRIDE;

private:
    QLabel * m_label;
    AccountMainWidget*m_mainWidget;
};

#endif
