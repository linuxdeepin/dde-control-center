/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QList>
#include <QDBusAbstractAdaptor>
#include <QPropertyAnimation>

#include "interfaces.h"
#include "modulemetadata.h"
#include "dbus/dbuscontrolcenter.h"
#include "dbus/dbusxmousearea.h"
#include "dbus/displayinterface.h"

class QStackedLayout;
class QKeyEvent;
class HomeScreen;
class ContentView;
class DBusControlCenterService;
class ControlCenterProxy;
class Frame: public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool hideInLeft READ isHideInLeft WRITE setHideInLeft NOTIFY hideInLeftChanged)
    Q_PROPERTY(bool autoHide READ autoHide WRITE setAutoHide NOTIFY autoHideChanged)
    Q_PROPERTY(bool visible READ isVisible NOTIFY visibleChanged)

public:
    Frame(QWidget *parent = 0);
    ~Frame();

    void show(bool imme = false);
    void hide(bool imme = false);
    inline bool isHideInLeft() const {return m_hideInLeft;}
    int visibleFrameXPos();
    bool autoHide() const;

public slots:
    void setHideInLeft(bool hideInLeft);
    void updateGeometry();
    void setAutoHide(bool autoHide);
    void toggle(bool inLeft);
    void selectModule(const QString &pluginId);
    bool isVisible() const;
    void loadContens();

signals:
    void hideInLeftChanged(bool hideInLeft);
    void xChanged();
    void autoHideChanged(bool autoHide);
    void visibleChanged(bool visible);

private slots:
    void globalMouseReleaseEvent(int button, int x, int y);
    void hideAndShowAnotherSide();
    void hideAndShowAnotherSideFinish();
    void showHelpDocument();

protected:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    HomeScreen *m_homeScreen;
    ContentView *m_contentView;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
    DBusXMouseArea *m_dbusXMouseArea;
    QWidget *m_centeralWidget;
    QWidget *m_centeralWarpper;
    QWidget *m_leftShadow;
    QWidget *m_rightShadow;
    ControlCenterProxy *m_controlProxy;
    QThread *m_pluginLoadThread = NULL;
    QString m_dbusFullScreenKey = QString();
    QPointer<const QScreen> primaryScreen;

    DisplayInterface *m_displayInter;
    QTimer *m_posAdjustTimer;

    bool m_visible = false;
    bool m_hideInLeft = true;
    bool m_autoHide = true;
};

#endif
