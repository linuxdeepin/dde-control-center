// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "networkplugin.h"
#include "networkpluginhelper.h"
#include "trayicon.h"
#include "networkdialog.h"

#include <DDBusSender>

#include <QTime>
#include <QMouseEvent>

#include <networkcontroller.h>

#define STATE_KEY "enabled"

NETWORKPLUGIN_USE_NAMESPACE

NetworkPlugin::NetworkPlugin(QObject *parent)
    : QObject(parent)
    , m_networkHelper(Q_NULLPTR)
    , m_networkDialog(Q_NULLPTR)
    , m_clickTime(-10000)
    , m_trayIcon(nullptr)
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
    if (m_networkHelper)
        return;

    m_networkDialog = new NetworkDialog(this);
    m_networkDialog->setServerName("dde-network-dialog" + QString::number(getuid()) + "dock");
    m_networkHelper.reset(new NetworkPluginHelper(m_networkDialog));

    if (!pluginIsDisable())
        loadPlugin();

    connect(m_networkDialog, &NetworkDialog::requestShow, this, &NetworkPlugin::showNetworkDialog);
}

void NetworkPlugin::invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked)
{
    Q_UNUSED(checked)

    if (itemKey == NETWORK_KEY)
        m_networkHelper->invokeMenuItem(menuId);
}

void NetworkPlugin::refreshIcon(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY)
        emit m_networkHelper->viewUpdate();
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
    if (m_networkHelper->needShowControlCenter()) {
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
        return m_networkHelper->contextMenu(true);

    return QString();
}

QWidget *NetworkPlugin::itemWidget(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY) {
        if (m_trayIcon.isNull()) {
            m_trayIcon.reset(new TrayIcon(m_networkHelper.data()));
        }
        return m_trayIcon.data();
    }

    return Q_NULLPTR;
}

QWidget *NetworkPlugin::itemTipsWidget(const QString &itemKey)
{
    if (itemKey == NETWORK_KEY && !m_networkDialog->panel()->isVisible())
        return m_networkHelper->itemTips();

    return Q_NULLPTR;
}

QWidget *NetworkPlugin::itemPopupApplet(const QString &itemKey)
{
    Q_UNUSED(itemKey);
    return m_networkDialog->panel();
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

void NetworkPlugin::showNetworkDialog()
{
    if (m_trayIcon.isNull() || m_networkDialog->panel()->isVisible())
        return;
    m_proxyInter->requestSetAppletVisible(this, NETWORK_KEY, true);
}
