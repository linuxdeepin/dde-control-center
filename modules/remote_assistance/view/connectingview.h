/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONNECTINGVIEW_H
#define CONNECTINGVIEW_H

#include "abstractview.h"

class ConnectingView : public AbstractView
{
    Q_OBJECT
public:
    ConnectingView(QWidget *p = nullptr);

signals:
    void cancel();

private:
    QWidget *createMainWidget() Q_DECL_OVERRIDE;

private slots:
    void onCancelButtonClicked();
};

#endif // CONNECTINGVIEW_H
