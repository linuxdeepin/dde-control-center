/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "networkplugin.h"
#include "networkpanel.h"
#include "trayicon.h"
#include "networkdialog.h"

#include <DDBusSender>

#include <QTime>

#include <networkcontroller.h>

#define STATE_KEY "enabled"

NetworkPlugin::NetworkPlugin(QObject *parent)
    : QObject(parent)
    , m_networkPanel(Q_NULLPTR)
    , m_networkDialog(Q_NULLPTR)
    , m_clickTime(-10000)
{
    NetworkController::setIPConflictCheck(true);
    QTranslator *translator = new QTranslator(this);
    translator->load(QString("/usr/share/dock-network-plugin/translations/dock-network-plugin_%1.qm").arg(QLocale::system().name()));
    QCoreApplication::installTranslator(translator);
}

NetworkPlugin::~NetworkPlugin()
{
}

const QString NetworkPlugin::pluginName() const
{
    return "network";
}

const QString NetworkPlugin::pluginDisplayName() const
{
    return tr("Network");
}

void NetworkPlugin::init(PluginProxyInterface *proxyInter)
{
    m_proxyInter = proxyInter;
    if (m_networkPanel)
        return;

    m_networkPanel.reset(new NetworkPanel);
    m_networkDialog = new NetworkDialog(this);
    QDBusConnection::sessionBus().connect("com.deepin.dde.lockFront", "/com/deepin/dde/lockFront", "com.deepin.dde.lockFront", "Visible", this, SLOT(lockFrontVisible(bool)));

    if (!pluginIsDisable())
        loadPlugin();
}

void NetworkPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(checked)

    if (itemKey == NETWORK_KEY)
        m_networkPanel->invokeMenuItem(menuId);
}

void NetworkPlugin::refreshIcon(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY)
        emit m_networkPanel->viewUpdate();
}

void NetworkPlugin::pluginStateSwitched()
{
    m_proxyInter->saveValue(this, STATE_KEY, pluginIsDisable());

    refreshPluginItemsVisible();
}

bool NetworkPlugin::pluginIsDisable()
{
    return !m_proxyInter->getValue(this, STATE_KEY, true).toBool();
}

const QString NetworkPlugin::itemCommand(const QString &itemKey)
{
    Q_UNUSED(itemKey)
    if (m_networkPanel->needShowControlCenter()) {
        return QString("dbus-send --print-reply "
                       "--dest=com.deepin.dde.ControlCenter "
                       "/com/deepin/dde/ControlCenter "
                       "com.deepin.dde.ControlCenter.ShowModule "
                       "\"string:network\"");
    }

    return QString();
}

const QString NetworkPlugin::itemContextMenu(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY)
        return m_networkPanel->contextMenu(true);

    return QString();
}

QWidget *NetworkPlugin::itemWidget(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY) {
        TrayIcon *trayIcon = new TrayIcon(m_networkPanel.data());
        connect(this, &NetworkPlugin::signalShowNetworkDialog, trayIcon, &TrayIcon::showNetworkDialog);
        connect(trayIcon, &TrayIcon::signalShowNetworkDialog, this, &NetworkPlugin::showNetworkDialog);
        QTimer::singleShot(100, this, &NetworkPlugin::updatePoint);
        return trayIcon;
    }

    return Q_NULLPTR;
}

QWidget *NetworkPlugin::itemTipsWidget(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY)
        return m_networkPanel->itemTips();

    return Q_NULLPTR;
}

QWidget *NetworkPlugin::itemPopupApplet(const QString &itemKey)
{
    int msec = QTime::currentTime().msecsSinceStartOfDay();
    if (!m_networkPanel->needShowControlCenter() && abs(msec - m_clickTime) > 200) {
        m_clickTime = msec;
        emit signalShowNetworkDialog();
    }
    return Q_NULLPTR;
}

int NetworkPlugin::itemSortKey(const QString &itemKey)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);
    return m_proxyInter->getValue(this, key, 3).toInt();
}

void NetworkPlugin::setSortKey(const QString &itemKey, const int order)
{
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(Dock::Efficient);
    m_proxyInter->saveValue(this, key, order);
}

void NetworkPlugin::pluginSettingsChanged()
{
    refreshPluginItemsVisible();
}

void NetworkPlugin::loadPlugin()
{
    m_proxyInter->itemAdded(this, NETWORK_KEY);
}

void NetworkPlugin::refreshPluginItemsVisible()
{
    if (pluginIsDisable())
        m_proxyInter->itemRemoved(this, NETWORK_KEY);
    else
        m_proxyInter->itemAdded(this, NETWORK_KEY);
}

void NetworkPlugin::positionChanged(const Dock::Position position)
{
    updatePoint();
}

void NetworkPlugin::lockFrontVisible(bool visible)
{
    if (!visible) {
        updatePoint();
    }
}

void NetworkPlugin::updatePoint()
{
    m_networkDialog->setSaveMode(true);
    emit signalShowNetworkDialog();
}

void NetworkPlugin::showNetworkDialog(QWidget *widget) const
{
    const QWidget *w = qobject_cast<QWidget *>(widget->parentWidget());
    const QWidget *parentWidget = w;
    Dock::Position position = Dock::Position::Bottom;
    QPoint point;
    while (w) {
        parentWidget = w;
        w = qobject_cast<QWidget *>(w->parentWidget());
    }
    if (parentWidget) {
        Dock::Position pos = qApp->property(PROP_POSITION).value<Dock::Position>();
        QPoint p = widget->rect().center();
        QRect rect = parentWidget->rect();
        switch (pos) {
        case Dock::Position::Top:
            p.ry() += rect.height() / 2;
            break;
        case Dock::Position::Bottom:
            p.ry() -= rect.height() / 2;
            break;
        case Dock::Position::Left:
            p.rx() += rect.width() / 2;
            break;
        case Dock::Position::Right:
            p.rx() -= rect.width() / 2;
            break;
        }
        position = pos;
        p = widget->mapToGlobal(p);
        point = p;
    }

    m_networkDialog->show(point.x(), point.y(), position);
}
