//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "controlcenterdbusadaptor.h"
#include "mainwindow.h"
#include "interface/moduleobject.h"

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QThread>
#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

#include <unistd.h>

using namespace DCC_NAMESPACE;

/*
 * Implementation of adaptor class DBusControlCenter
 */

ControlCenterDBusAdaptor::ControlCenterDBusAdaptor(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
{
}

ControlCenterDBusAdaptor::~ControlCenterDBusAdaptor()
{
}

MainWindow *ControlCenterDBusAdaptor::parent() const
{
    return static_cast<MainWindow *>(QObject::parent());
}

const QRect ControlCenterDBusAdaptor::rect() const
{
    return parent()->geometry();
}

void ControlCenterDBusAdaptor::Exit()
{
    pid_t pid = getpid();
    qDebug() << "exit pid:" << pid;
    QCoreApplication::quit();
}

void ControlCenterDBusAdaptor::Hide()
{
    parent()->hide();
}

void ControlCenterDBusAdaptor::Show()
{
    if (parent()->isMinimized() || !parent()->isVisible())
        parent()->showNormal();

    parent()->activateWindow();
}

void ControlCenterDBusAdaptor::ShowHome()
{
    parent()->showPage("", MainWindow::UrlType::Name);
    Show();
}

void ControlCenterDBusAdaptor::ShowPage(const QString &url)
{
    parent()->showPage(url);
    Show();
}

void ControlCenterDBusAdaptor::Toggle()
{
    parent()->setVisible(!parent()->isVisible());
    if (parent()->isVisible())
        parent()->activateWindow();
}

QString ControlCenterDBusAdaptor::GetAllModule()
{
    return parent()->getAllModule();
}

DBusControlCenterGrandSearchService::DBusControlCenterGrandSearchService(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
    , m_autoExitTimer(new QTimer(this))
{
    m_autoExitTimer->setInterval(10000);
    m_autoExitTimer->setSingleShot(true);
    connect(m_autoExitTimer, &QTimer::timeout, this, [this]() {
        //当主界面show出来之后不再执行自动退出
        if (!this->parent()->isVisible())
            QCoreApplication::quit();
    });
    m_autoExitTimer->start();
}

DBusControlCenterGrandSearchService::~DBusControlCenterGrandSearchService()
{
}

MainWindow *DBusControlCenterGrandSearchService::parent() const
{
    return static_cast<MainWindow *>(QObject::parent());
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
