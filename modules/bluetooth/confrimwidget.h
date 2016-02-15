/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONFRIMWIDGET_H
#define CONFRIMWIDGET_H

#include <QWidget>

#include <dtextbutton.h>

DWIDGET_USE_NAMESPACE

class ConfrimWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfrimWidget(QWidget *parent = 0);

    void setDisconnectVisible(bool visible);

signals:
    void ignore();
    void disconnect();

private:
    DTextButton *m_disconnect;
    DTextButton *m_ignore;
};

#endif // CONFRIMWIDGET_H
