/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GENERATINGVIEW_H
#define GENERATINGVIEW_H

#include "abstractview.h"

class GeneratingView : public AbstractView
{
    Q_OBJECT
public:
    GeneratingView(QWidget *p = nullptr);

signals:
    void cancel();

private slots:
    void onCancelButtonClicked();

private:
    QWidget *createMainWidget();
};

#endif // GENERATINGVIEW_H
