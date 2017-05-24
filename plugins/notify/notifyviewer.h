/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef VIEWER_H
#define VIEWER_H

#define ICON_SIZE 48

#include "notifybody.h"

#include <QObject>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <dimagebutton.h>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QStyleOption>
#include <QPainter>

class NumButton;
DWIDGET_USE_NAMESPACE
class Viewer : public QFrame
{
    Q_OBJECT
public:
    explicit Viewer(const QJsonObject &value, QWidget *parent = 0);
    void setAppName(const QString &s);
    void setAppIcon(const QString &s);
    void setAppBody(const QString &s);
    void setAppTime(const QString &s);
    void setAppId(const QString &id);

signals:
    void requestClose(const QString &id);

public slots:
    void onClose();

private slots:
    void onPlayCloseAnimation();
    void onAnimationFinished();

protected:
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    QLabel *m_appName, *m_time;
    NotifyBody *m_body;
    DImageButton *m_close;
    QLabel      *m_appIcon;
    QHBoxLayout *m_mainlayout;
    QString m_id;
};

#endif // VIEWER_H
