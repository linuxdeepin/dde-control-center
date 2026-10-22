// SPDX-FileCopyrightText: 2016 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bluetoothitem.h"

#include "imageutil.h"
#include "tipswidget.h"
#include "componments/bluetoothapplet.h"
#include "constants.h"
#include "quickpanelwidget.h"


#include <DApplication>
#include <DGuiApplicationHelper>

#include <QPainter>
#include <QProcess>

// menu actions
#define SHIFT "shift"
#define SETTINGS "settings"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

using namespace Dock;

BluetoothItem::BluetoothItem(AdaptersManager *adapterManager, QWidget *parent)
    : QWidget(parent)
    , m_tipsLabel(new TipsWidget(this))
    , m_applet(new BluetoothApplet(adapterManager, this))
    , m_quickPanel(new QuickPanelWidget(this))
    , m_iconWidget(new CommonIconButton(this))
    , m_devState(Device::State::StateUnavailable)
    , m_adapterPowered(m_applet->poweredInitState())
{
    setAccessibleName("BluetoothPluginItem");
    m_applet->setVisible(false);
    m_tipsLabel->setVisible(false);
    m_quickPanel->setVisible(false);
    m_quickPanel->setText(tr("Bluetooth"));
    m_iconWidget->setFixedSize(Dock::DOCK_PLUGIN_ITEM_FIXED_SIZE);
    refreshIcon();
    refreshTips();

    connect(m_applet, &BluetoothApplet::powerChanged, this, [ & ](bool powered) {
        m_adapterPowered = powered;
        refreshIcon();
        refreshTips();
    });
    connect(m_applet, &BluetoothApplet::deviceStateChanged, this, [ & ](const Device *device) {
        m_devState = device->state();
        refreshIcon();
        refreshTips();
    });
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &BluetoothItem::refreshIcon);
    connect(m_applet, &BluetoothApplet::noAdapter, this, [this] {
        if (m_applet->isVisible()) {
            m_applet->setVisible(false);
        }
        Q_EMIT noAdapter();
    });
    connect(m_applet, &BluetoothApplet::justHasAdapter, this, &BluetoothItem::justHasAdapter);
    connect(m_applet, &BluetoothApplet::requestHideApplet, this, &BluetoothItem::requestHideApplet);
    connect(m_quickPanel, &QuickPanelWidget::panelClicked, this, &BluetoothItem::requestExpand);
    connect(m_quickPanel, &QuickPanelWidget::iconClicked, this, [this]() {
        invokeMenuItem(SHIFT, true);
    });
}

QWidget *BluetoothItem::tipsWidget()
{
    refreshTips();
    return m_tipsLabel;
}

BluetoothApplet *BluetoothItem::popupApplet()
{
    if (m_applet && m_applet->hasAdapter())
        m_applet->setAdapterRefresh();
    return m_applet->hasAdapter() ? m_applet : nullptr;
}

QWidget *BluetoothItem::quickPanel()
{
    return m_quickPanel;
}

const QString BluetoothItem::contextMenu() const
{
    QList<QVariant> items;
    if (m_applet->hasAdapter()) {
        items.reserve(2);

        QMap<QString, QVariant> shift;
        shift["itemId"] = SHIFT;
        if (m_adapterPowered)
            shift["itemText"] = tr("Turn off");
        else
            shift["itemText"] = tr("Turn on");
        shift["isActive"] = !m_applet->airplaneModeEnable();
        items.push_back(shift);

        QMap<QString, QVariant> settings;
        settings["itemId"] = SETTINGS;
        settings["itemText"] = tr("Bluetooth settings");
        settings["isActive"] = true;
        items.push_back(settings);

        QMap<QString, QVariant> menu;
        menu["items"] = items;
        menu["checkableMenu"] = false;
        menu["singleCheck"] = false;
        return QJsonDocument::fromVariant(menu).toJson();
    }
    return QByteArray();
}

void BluetoothItem::invokeMenuItem(const QString menuId, const bool checked)
{
    Q_UNUSED(checked);

    if (menuId == SHIFT) {
        if (m_applet->airplaneModeEnable()) {
            return;
        }
        m_applet->setAdapterPowered(!m_adapterPowered);
    } else if (menuId == SETTINGS) {
        QStringList args {"--by-user", "org.deepin.dde.control-center", "--", "-p", "device/bluetooth"};
        QProcess::startDetached("dde-am", args);
        Q_EMIT requestHideApplet();
    }
}

void BluetoothItem::refreshIcon()
{
    QString stateString;
    QString iconString;

    if (m_adapterPowered) {
        if (m_applet->connectedDevicesName().isEmpty()) {
            stateString = "disconnect";
        } else {
            stateString = "active";
        }
    } else {
        stateString = "disable";
    }

    auto icon = QIcon::fromTheme(QString("bluetooth-%1-symbolic").arg(stateString));
    m_quickPanel->setIcon(icon);
    m_iconWidget->setIcon(icon);
    m_quickPanel->setActive(m_adapterPowered);
    update();
}

void BluetoothItem::refreshTips()
{
    QString tipsText;
    QString description;

    if (m_adapterPowered) {
        if (!m_applet->connectedDevicesName().isEmpty() && m_devState != Device::StateAvailable) {
            QStringList textList;
            const auto &connectedNames = m_applet->connectedDevicesName();
            for (const QString &devName : connectedNames) {
                textList << tr("%1 connected").arg(devName);
            }
            m_tipsLabel->setTextList(textList);
            if (connectedNames.size() == 1) {
                m_quickPanel->setDescription(connectedNames.first());
            } else {
                m_quickPanel->setDescription(tr("Connected %1").arg(connectedNames.size()));
            }
            return;
        }
        if (m_devState == Device::StateAvailable) {
            tipsText = tr("Connecting...");
            description = tipsText;
        } else {
            tipsText = tr("Not connected");
            description = tipsText;
        }
    } else {
        tipsText = tr("Turned off");
        description = tr("Off");
    }

    m_tipsLabel->setText(tipsText);
    m_quickPanel->setDescription(description);
}

bool BluetoothItem::hasAdapter()
{
    return m_applet->hasAdapter();
}

void BluetoothItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    // 保持横纵比
    if (position == Dock::Bottom || position == Dock::Top) {
        setMaximumWidth(height());
        setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        setMaximumHeight(width());
        setMaximumWidth(QWIDGETSIZE_MAX);
    }

    refreshIcon();
}
