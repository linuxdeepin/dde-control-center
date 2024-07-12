// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "controlcenterdbusadaptor.h"

#include "dccmanager.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QTimer>
#include <QWindow>

#include <unistd.h>

using namespace dccV25;
const QString DBusProperties = "org.freedesktop.DBus.Properties";
const QString DBusPropertiesChanged = "PropertiesChanged";

/*
 * Implementation of adaptor class DBusControlCenter
 */

ControlCenterDBusAdaptor::ControlCenterDBusAdaptor(DccManager *parent)
    : QDBusAbstractAdaptor(parent)
{
    parent->mainWindow()->installEventFilter(this);
    connect(parent, &DccManager::pathChanged, this, &ControlCenterDBusAdaptor::updatePage);
}

ControlCenterDBusAdaptor::~ControlCenterDBusAdaptor() { }

DccManager *ControlCenterDBusAdaptor::parent() const
{
    return static_cast<DccManager *>(QObject::parent());
}

const QRect ControlCenterDBusAdaptor::rect() const
{
    return parent()->mainWindow()->geometry();
}

const QString ControlCenterDBusAdaptor::page() const
{
    QStringList path;
    for (auto obj : parent()->currentObjects()) {
        path.append(obj->name());
    }
    path.takeFirst();
    return path.join("/");
}

const QString ControlCenterDBusAdaptor::path() const
{
    QStringList path;
    for (auto obj : parent()->currentObjects()) {
        if (!obj->displayName().isEmpty()) {
            path.append(obj->displayName());
        }
    }
    return path.join("/");
}

void ControlCenterDBusAdaptor::Exit()
{
    pid_t pid = getpid();
    qDebug() << "exit pid:" << pid << QCoreApplication::applicationPid();
    QCoreApplication::quit();
}

void ControlCenterDBusAdaptor::Hide()
{
    parent()->mainWindow()->hide();
}

void ControlCenterDBusAdaptor::Show()
{
    QWindow *w = parent()->mainWindow();
    if (w->windowStates() == Qt::WindowMinimized || !w->isVisible())
        w->showNormal();
    w->requestActivate();
}

void ControlCenterDBusAdaptor::ShowHome()
{
    parent()->showPage("");
    Show();
}

void ControlCenterDBusAdaptor::ShowPage(const QString &url)
{
    parent()->showPage(url);
    Show();
}

void ControlCenterDBusAdaptor::Toggle()
{
    QWindow *w = parent()->mainWindow();
    w->setVisible(!w->isVisible());
    if (w->isVisible())
        w->requestActivate();
}

QString ControlCenterDBusAdaptor::GetAllModule()
{
    return "parent()->getAllModule()";
}

bool ControlCenterDBusAdaptor::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Move:
    case QEvent::Resize:
        updateRect();
        break;
    default:
        break;
    }
    return QDBusAbstractAdaptor::eventFilter(obj, event);
}

void ControlCenterDBusAdaptor::updatePage()
{
    QDBusMessage msg = QDBusMessage::createSignal(DccDBusPath, DBusProperties, DBusPropertiesChanged);
    msg << DccDBusInterface << QVariantMap({ { "Page", page() }, { "Path", path() } }) << QStringList();
    QDBusConnection::sessionBus().asyncCall(msg);
}

void ControlCenterDBusAdaptor::updateRect()
{
    QDBusMessage msg = QDBusMessage::createSignal(DccDBusPath, DBusProperties, DBusPropertiesChanged);
    msg << DccDBusInterface << QVariantMap({ { "Rect", rect() } }) << QStringList();
    QDBusConnection::sessionBus().asyncCall(msg);
}

DBusControlCenterGrandSearchService::DBusControlCenterGrandSearchService(DccManager *parent)
    : QDBusAbstractAdaptor(parent)
    , m_autoExitTimer(new QTimer(this))
{
    m_autoExitTimer->setInterval(10000);
    m_autoExitTimer->setSingleShot(true);
    connect(m_autoExitTimer, &QTimer::timeout, this, [this]() {
        // 当主界面show出来之后不再执行自动退出
        if (!this->parent()->mainWindow()->isVisible())
            QCoreApplication::quit();
    });
    m_autoExitTimer->start();
}

DBusControlCenterGrandSearchService::~DBusControlCenterGrandSearchService() { }

DccManager *DBusControlCenterGrandSearchService::parent() const
{
    return static_cast<DccManager *>(QObject::parent());
}

// 匹配搜索结果
QString DBusControlCenterGrandSearchService::Search(const QString json)
{
    QString val = parent()->search(json);
    m_autoExitTimer->start();
    return val;
}

// 停止搜索
bool DBusControlCenterGrandSearchService::Stop(const QString json)
{
    bool val = parent()->stop(json);
    m_autoExitTimer->start();
    return val;
}

// 执行搜索
bool DBusControlCenterGrandSearchService::Action(const QString json)
{
    bool val = parent()->action(json);
    m_autoExitTimer->start();
    return val;
}
