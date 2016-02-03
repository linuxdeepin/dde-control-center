/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TITLEANDWIDGET_H
#define TITLEANDWIDGET_H

#include <QLabel>

class TitleAndWidget : public QLabel
{
    Q_OBJECT
public:
    explicit TitleAndWidget(QWidget *w, bool showSeparator = true, QWidget *parent = 0);

    void setTitleVisible(bool arg);

private:
    bool m_titleVisible = true;
    QString m_title;
};

#endif // TITLEANDWIDGET_H
