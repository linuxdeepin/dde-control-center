/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TIMEZONECTRLWIDGET_H
#define TIMEZONECTRLWIDGET_H

#include <QWidget>

#include "imagenamebutton.h"
#include "dynamiclabel.h"

#include <dtextbutton.h>

DWIDGET_USE_NAMESPACE

class TimezoneCtrlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimezoneCtrlWidget(QWidget *parent = 0);

signals:
    void removeTimezone();
    void removeAccept();
    void addTimezone();
    void addTimezoneAccept();
    void addTimezoneCancel();

public slots:
    void toNormalMode();
    void toDelTimezoneMode();
    void toAddTimezoneMode();
    void setAcceptOrCancel(bool accept);
    void setListNums(int nums);

private:
    DynamicLabel *m_TipsLabel;
    ImageNameButton *m_addBtn;
    ImageNameButton *m_delBtn;
    DTextButton *m_cancelBtn;
    DTextButton *m_confirmBtn;
    DTextButton *m_addTimezoneBtn;

    int m_listNums = 0;
};

#endif // TIMEZONECTRLWIDGET_H
