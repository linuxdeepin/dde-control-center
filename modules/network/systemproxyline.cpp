/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dlineedit.h>
#include <dlabel.h>
#include <dboxwidget.h>
#include <dthememanager.h>

#include "systemproxyline.h"

SystemProxyLine::SystemProxyLine(const QString &title, const QString &proxyType,
                                 DBusNetwork *dbus, QWidget *parent) :
    DHeaderLine(parent)
{
    D_THEME_INIT_WIDGET(SystemProxyWidget)

    setTitle(title);

    DLineEdit *edit_host = new DLineEdit;
    DLineEdit *edit_port = new DLineEdit;
    DLabel *label_port = new DLabel(tr("Port"));
    DHBoxWidget *h_boxwidget = new DHBoxWidget;

    edit_port->setFixedWidth(40);

    ASYN_CALL(dbus->GetProxy(proxyType), {
                  if(args.count() > 1) {
                    edit_host->setText(args[0].toString());
                    edit_port->setText(args[1].toString());
                  }
              }, edit_host, edit_port)

    h_boxwidget->layout()->setSpacing(5);
    h_boxwidget->addWidget(edit_host);
    h_boxwidget->addWidget(label_port);
    h_boxwidget->addWidget(edit_port);

    auto updateProxyInfo = [this, dbus, edit_host, edit_port, proxyType] {
        dbus->SetProxy(proxyType, edit_host->text(),
                       edit_port->text().isEmpty() ? "0" : edit_port->text());
    };

    connect(edit_host, &DLineEdit::editingFinished, this, updateProxyInfo);
    connect(edit_port, &DLineEdit::editingFinished, this, updateProxyInfo);

    setLeftMargin(15);
    setRightMargin(15);
    setContent(h_boxwidget);
}
