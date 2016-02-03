/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

#include "searchlist.h"

class ToolTip : public QLabel, public SearchItem
{
    Q_OBJECT

    Q_PROPERTY(bool timeoutDestroy READ timeoutDestroy WRITE setTimeoutDestroy)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)

public:
    explicit ToolTip(QWidget *parent = 0);

    QStringList keyWords() const Q_DECL_OVERRIDE;
    void setData(const QVariant &datas) Q_DECL_OVERRIDE;
    QVariant getData() Q_DECL_OVERRIDE;
    QWidget *widget() const Q_DECL_OVERRIDE;

    bool timeoutDestroy() const;
    int timeout() const;

public slots:
    void setTimeoutDestroy(bool timeoutDestroy);
    void setTimeout(int timeout);
    void expansion();
    void contraction();

signals:
    void expanded();
    void contracted();

private:
    QWidget *m_widget;
    bool m_timeoutDestroy;
    int m_timeout;
    QTimer *m_timer;
    QPropertyAnimation *m_animation;
};

#endif // TOOLTIP_H
