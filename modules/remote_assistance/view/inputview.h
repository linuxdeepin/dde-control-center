/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef INPUTVIEW_H
#define INPUTVIEW_H

#include <QString>
#include "abstractview.h"
#include <libdui/libdui_global.h>

class QLineEdit;
class QLabel;
class QRegExpValidator;

DUI_BEGIN_NAMESPACE
class DTextButton;
DUI_END_NAMESPACE

class InputView : public AbstractView
{
    Q_OBJECT
public:
    InputView(QWidget *p = nullptr);
    void focus();

signals:
    void cancel();
    void connect(QString);

private slots:
    void emitConnect();
    void connectToClient();

private:
    QWidget *createMainWidget() Q_DECL_OVERRIDE;
    QLineEdit *m_tokenEdit;
    QLabel *m_tip;
    QRegExpValidator *m_validator;
    DUI_NAMESPACE::DTextButton *m_connectButton;
};

#endif // INPUTVIEW_H
