/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DATECONTROLWIDGET_H
#define DATECONTROLWIDGET_H

#include <QWidget>
#include <QDate>

#include <libdui/dimagebutton.h>
#include <libdui/dtextbutton.h>

#include "dynamiclabel.h"

DUI_USE_NAMESPACE

class DateControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DateControlWidget(QWidget *parent = 0);

signals:
    void applyDate();

public slots:
    void toNormalMode();
    void toConfirmMode();

private:
    DynamicLabel *m_tipsLabel;
    DImageButton *m_ctrlBtn;
    DTextButton *m_confirmBtn;
    DTextButton *m_cancelBtn;
};

#endif // DATECONTROLWIDGET_H
