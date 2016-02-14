/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef REMOTE_ASSISTANCE_H
#define REMOTE_ASSISTANCE_H

#include <QObject>
#include <QScopedPointer>
#include <libdui_global.h>
#include "interfaces.h"
#include "dbus/manager.h"

QT_BEGIN_NAMESPACE
class QFrame;
QT_END_NAMESPACE

DUI_BEGIN_NAMESPACE
class DStackWidget;
DUI_END_NAMESPACE

enum class ViewPanel
{
    Main,
    Share,
    Access,
};

class RemoteAssistance: public QObject, public ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "remote_assistance.json")
    Q_INTERFACES(ModuleInterface)
public:
    RemoteAssistance();
    ~RemoteAssistance();
    QFrame *getContent() Q_DECL_OVERRIDE;

public slots:
    void changePanel(ViewPanel);

private:
    class Impl
    {
    public:
        Impl(RemoteAssistance *, com::deepin::daemon::Remoting::Manager *);
        ~Impl();
        inline void popView(QWidget *w = nullptr, bool isDelete = true, int count = 1, bool enableTransition = true);
        inline void pushView(QWidget *w, bool enableTransition = true);
        void initPanel();
        void changePanel(ViewPanel);

    public:
        QWidget *getPanel(ViewPanel);

        RemoteAssistance *m_pub;
        com::deepin::daemon::Remoting::Manager *m_manager;
        QFrame *m_view; // NB: the m_view will be reparented, should not delete it in dtor.
        DUI_NAMESPACE::DStackWidget *m_stackWidget; // this is child of m_view.
        QWidget *m_panel;
        ViewPanel m_viewType;
    };

    QScopedPointer<Impl> m_impl;

private slots:
    void onAnimationEnd();
    void hide();
};

#endif /* end of include guard: REMOTE_ASSISTANCE_H */
