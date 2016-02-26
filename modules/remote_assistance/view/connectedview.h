/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SHARINGVIEW_H
#define SHARINGVIEW_H

#include "abstractview.h"

class QLabel;

class ConnectedView : public AbstractView
{
    Q_OBJECT
public:
    ConnectedView(QWidget *p = nullptr);
    ConnectedView *setText(const QString &);

signals:
    void disconnect();

private:
    QWidget *createMainWidget() Q_DECL_OVERRIDE;

private slots:
    void onDisconnectButtonClicked();

private:
    QLabel *m_text;
};

#endif // SHARINGVIEW_H
