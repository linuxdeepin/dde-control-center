// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "dbuscontrolcenterservice.h"
#include "window/mainwindow.h"

#include "modules/display/displaymodel.h"
#include "modules/display/displayworker.h"

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QThread>
#include <QGuiApplication>
#include <qpa/qplatformwindow.h>
#include <QScreen>
#include <QString>

#include <unistd.h>

/*
 * Implementation of adaptor class DBusControlCenter
 */

using namespace DCC_NAMESPACE;

DBusControlCenterService::DBusControlCenterService(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
    , m_toggleProcessed(true)
{

}

DBusControlCenterService::~DBusControlCenterService()
{

}

MainWindow *DBusControlCenterService::parent() const
{
    return static_cast<MainWindow *>(QObject::parent());
}

bool DBusControlCenterService::showInRight() const
{
    return true;
}

const QRect DBusControlCenterService::rect() const
{
    if (parent()->windowHandle() && parent()->windowHandle()->handle()) {
        return parent()->windowHandle()->handle()->geometry();
    }

    return parent()->geometry();
}

void DBusControlCenterService::exitProc()
{
    pid_t pid = getpid();
    qDebug() << "exit pid:" << pid;
    QCoreApplication::quit();
}

void DBusControlCenterService::Hide()
{
    // handle method call com.deepin.dde.ControlCenter.Hide
    parent()->hide();
}

void DBusControlCenterService::HideImmediately()
{
    // handle method call com.deepin.dde.ControlCenter.HideImmediately
//    parent()->hideImmediately();
}

void DBusControlCenterService::Show()
{
    // handle method call com.deepin.dde.ControlCenter.Show
#ifdef DISABLE_MAIN_PAGE
    parent()->showSettingsPage(QString(), QString());
#else
    parent()->initAllModule();

    parent()->raise();

    if (!qgetenv("WAYLAND_DISPLAY").isEmpty()) {
        QElapsedTimer et;
        et.start();
        while (!parent()->primaryScreen() && et.elapsed() < 2000) {
            QThread::msleep(50);
            QCoreApplication::processEvents();
        }
    }

    if (parent()->isMinimized() || !parent()->isVisible())
        parent()->showNormal();

    parent()->activateWindow();
#endif
}

void DBusControlCenterService::ShowImmediately()
{
    // handle method call com.deepin.dde.ControlCenter.ShowImmediately
#ifdef DISABLE_MAIN_PAGE
    parent()->showSettingsPage(QString(), QString());
#else
    if (parent()->isMinimized())
        parent()->showNormal();

    parent()->activateWindow();
#endif
}

void DBusControlCenterService::ShowHome()
{
    parent()->popWidget();
}

void DBusControlCenterService::ShowModule(const QString &module)
{
    ShowPage(module, "");
}

void DBusControlCenterService::ShowPage(const QString &module, const QString &page)
{
    parent()->initAllModule(module);

    static bool firstEnter = true;
    QStringList delayModule{"bluetooth"};
    if (delayModule.contains(module) && firstEnter) {
        firstEnter = false;
        //此时对应了dbus调用显示控制中心需要dbus同步方式数据
    }

    parent()->showModulePage(module, page, false);
}

void DBusControlCenterService::Toggle()
{
    Show();
}

void DBusControlCenterService::ToggleInLeft()
{
    // handle method call com.deepin.dde.ControlCenter.ToggleInLeft
    Toggle();
}

bool DBusControlCenterService::isNetworkCanShowPassword()
{
    // handle method call com.deepin.dde.ControlCenter.isNetworkCanShowPassword
    //return parent()->isNetworkCanShowPassword();

    return false; // for remove gcc warning.
}

bool DBusControlCenterService::isModuleAvailable(const QString &m)
{
    return parent()->isModuleAvailable(m);
}


DBusControlCenterGrandSearchService::DBusControlCenterGrandSearchService(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
    , m_autoExitTimer(new QTimer(this))
{
    this->parent()->initAllModule();
    m_autoExitTimer->setInterval(10000);
    m_autoExitTimer->setSingleShot(true);
    connect(m_autoExitTimer, &QTimer::timeout, this, [=]() {
        //当主界面show出来之后不再执行自动退出
        if(!this->parent()->isVisible())
            QCoreApplication::quit();
    });
    m_autoExitTimer->start();
}

DBusControlCenterGrandSearchService::~DBusControlCenterGrandSearchService()
{

}

//匹配搜索结果
QString DBusControlCenterGrandSearchService::Search(const QString json)
{
    QString val = parent()->GrandSearchSearch(json);
    m_autoExitTimer->start();
    return val;
}

//停止搜索
bool DBusControlCenterGrandSearchService::Stop(const QString json)
{
    bool val = parent()->GrandSearchStop(json);

    m_autoExitTimer->start();

    return val;
}

//执行搜索
bool DBusControlCenterGrandSearchService::Action(const QString json)
{
    bool val = parent()->GrandSearchAction(json);

    m_autoExitTimer->start();

    return val;
}

MainWindow *DBusControlCenterGrandSearchService::parent() const
{
    return static_cast<MainWindow *>(QObject::parent());
}
