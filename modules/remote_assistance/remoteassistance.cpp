/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QFrame>
#include <QDBusConnection>
#include <QVBoxLayout>

#include <dstackwidget.h>

#include "../../frame/constants.h"
#include "../../interfaces/controlcenterproxyinterface.h"
#include "remoteassistance.h"
#include "dbus/manager.h"
#include "controller/access.h"
#include "controller/share.h"
#include "view/accesspanel.h"
#include "view/mainpanel.h"
#include "view/sharepanel.h"

namespace ManagerState {
    enum {
        Uninitialized,
        Client,
        Server,
    };
}

DWIDGET_USE_NAMESPACE

RemoteAssistance::Impl::Impl(RemoteAssistance* pub, com::deepin::daemon::Remoting::Manager* manager)
    : m_pub(pub),
      m_manager(manager),
      m_view(new QFrame),
      m_stackWidget(new DStackWidget(m_view))
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Remote Assistance"));

    connect(m_stackWidget->transition()->animation(), SIGNAL(finished()), pub, SLOT(onAnimationEnd()));
    m_stackWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    m_stackWidget->setFixedWidth(DCC::ModuleContentWidth);
}

RemoteAssistance::Impl::~Impl()
{
    m_manager->deleteLater();
}

void RemoteAssistance::Impl::initPanel()
{
    QDBusPendingReply<int> reply = m_manager->GetStatus();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << reply.error();
        return ;
    }
    pushView(getPanel(ViewPanel::Main), false);
    switch (reply.value()) {
    case ManagerState::Uninitialized:
        m_viewType = ViewPanel::Main;
        break;
    case ManagerState::Client:
        m_viewType = ViewPanel::Access;
        pushView(getPanel(ViewPanel::Access), false);
        break;
    case ManagerState::Server:
        m_viewType = ViewPanel::Share;
        pushView(getPanel(ViewPanel::Share), false);
        break;
    }
}

QWidget* RemoteAssistance::Impl::getPanel(ViewPanel v)
{
    switch (v) {
    case ViewPanel::Main: {
        // MainPanel should be created only once.
        qDebug() << "create Main Panel";
        QWidget* m_mainPanel = new MainPanel(m_manager);
        QObject::connect(m_mainPanel, SIGNAL(changePanel(ViewPanel)), m_pub, SLOT(changePanel(ViewPanel)));
        return m_mainPanel;
    }
    case ViewPanel::Access: {
        qDebug() << "create Access Panel";
        auto client = new com::deepin::daemon::Remoting::Client("com.deepin.daemon.Remoting.Client", "/com/deepin/daemon/Remoting/Client", QDBusConnection::sessionBus());
        auto controller = new AccessController(m_manager, client);
        QWidget* m_accessPanel = new AccessPanel(controller);
        QObject::connect(m_accessPanel, SIGNAL(changePanel(ViewPanel)), m_pub, SLOT(changePanel(ViewPanel)));
        QObject::connect(m_accessPanel, SIGNAL(connected()), m_pub, SLOT(hide()));
        return m_accessPanel;
    }
    case ViewPanel::Share: {
        qDebug() << "create Share Panel";
        auto server = new com::deepin::daemon::Remoting::Server("com.deepin.daemon.Remoting.Server", "/com/deepin/daemon/Remoting/Server", QDBusConnection::sessionBus());
        auto controller  = new ShareController(m_manager, server);
        QWidget* m_sharePanel = new SharePanel(controller);
        QObject::connect(m_sharePanel, SIGNAL(changePanel(ViewPanel)), m_pub, SLOT(changePanel(ViewPanel)));
        return m_sharePanel;
    }
    }
    throw "[RemoteAssistance::Impl::getPanel] should not reach here";
}

void RemoteAssistance::Impl::changePanel(ViewPanel v)
{
    m_viewType = v;
    if (m_stackWidget->depth() > 1) {
        popView();
        return;
    }

    QWidget* panel = getPanel(v);
    pushView(panel);
}

void RemoteAssistance::onAnimationEnd()
{
    qDebug() << "onAnimationEnd();";
    if (m_impl->m_viewType == ViewPanel::Access) {
        qobject_cast<AccessPanel*>(m_impl->m_panel)->focus();
    }
}

void RemoteAssistance::changePanel(ViewPanel v)
{
    m_impl->changePanel(v);
}

inline void RemoteAssistance::Impl::pushView(QWidget* w, bool enableTransition)
{
    qDebug() << "push new panel" << w->objectName();
    m_panel = w;
    m_stackWidget->pushWidget(w, enableTransition);
    if (!enableTransition) {
        m_pub->onAnimationEnd();
    }
}

inline void RemoteAssistance::Impl::popView(QWidget* w, bool isDelete, int count, bool enableTransition)
{
    qDebug() << "pop last panel" << m_panel->objectName() << ", depth" << m_stackWidget->depth();
    m_stackWidget->popWidget(w, isDelete, count, enableTransition);
    m_panel = m_stackWidget->currentWidget();
    qDebug() << "current panel is" << m_panel->objectName();
}

RemoteAssistance::RemoteAssistance()
    : QObject(),
      ModuleInterface(),
      m_impl(new Impl(this, new com::deepin::daemon::Remoting::Manager("com.deepin.daemon.Remoting.Manager", "/com/deepin/daemon/Remoting/Manager", QDBusConnection::sessionBus())))
{
    m_impl->initPanel();
}

RemoteAssistance::~RemoteAssistance()
{
}

QFrame* RemoteAssistance::getContent()
{
    return m_impl->m_view;
}

void RemoteAssistance::hide()
{
    m_controlCenterProxy->hide();
}

