// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wirelesscastingitem.h"

#include "quickpanelwidget.h"
#include "wirelesscastingapplet.h"
#include "wirelesscastingmodel.h"
#include "displaymodel.h"

#include <constants.h>


#include <DGuiApplicationHelper>

#include <QPixmap>
#include <QProcess>

namespace dde {
namespace wirelesscasting {

#define SETTINGS "settings"

WirelessCastingItem::WirelessCastingItem(QWidget *parent)
    : QWidget(parent)
    , m_model(new WirelessCastingModel(this))
    , m_displayMode(new DisplayModel(this))
    , m_quickPanel(new QuickPanelWidget())
    , m_appletWidget(new WirelessCastingApplet(m_model, m_displayMode, this))
    , m_trayIcon(new QIcon())
    , m_tips(nullptr)
{
    init();
    auto syncState = [this] {
        bool const canCasting = m_canCasting;
#ifdef WIRELESS_CASTING_ENABLED
        WirelessCastingModel::CastingState const state = m_model->state();
        m_canCasting = (WirelessCastingModel::NoWirelessDevice != state && WirelessCastingModel::NotSupportP2P != state) || m_model->multiscreensFlag();
#else
        m_canCasting = m_model->multiscreensFlag();
#endif
        if (canCasting != m_canCasting)
            Q_EMIT canCastingChanged(m_canCasting);

        *m_trayIcon = QIcon::fromTheme("network-display-failed-symbolic");
        m_quickPanel->setIcon(QIcon::fromTheme("network-display-failed-symbolic"));
        m_quickPanel->setActive(false);
        if (m_model->state() == WirelessCastingModel::Connected && m_displayMode->screens().size() > 1) {
            m_quickPanel->setDescription(tr("Multiple services started"));
        } else if (m_model->state() != WirelessCastingModel::Connected && m_displayMode->screens().size() > 1) {
            m_quickPanel->setDescription(m_displayMode->currentMode());
        } else if (WirelessCastingModel::Connected != m_model->state()) {
            if (m_model->connectState()) {
                m_quickPanel->setDescription(tr("Connecting"));
            } else {
                m_quickPanel->setDescription(tr("Not connected"));
            }
        } else {
            *m_trayIcon = QIcon::fromTheme("network-display-succeed-symbolic");
            m_quickPanel->setIcon(QIcon::fromTheme("network-display-succeed-symbolic"));
            m_quickPanel->setActive(true);
            m_quickPanel->setDescription(m_model->curMonitorName());
        }
        update();
    };
    connect(m_displayMode, &DisplayModel::displayModeChanged, this, syncState);
    connect(m_displayMode, &DisplayModel::primaryScreenChanged, this, syncState);
    connect(m_displayMode, &DisplayModel::currentModeChanged, this, syncState);
    connect(m_model, &WirelessCastingModel::stateChanged, this, [=](WirelessCastingModel::CastingState state) {
        Q_UNUSED(state)
        syncState();
    });
    connect(m_model, &WirelessCastingModel::connectStateChanged, this, syncState);
    connect(m_model, &WirelessCastingModel::multiscreensFlagChanged, this, syncState);
    syncState();
    connect(m_quickPanel, &QuickPanelWidget::panelClicked, this, &WirelessCastingItem::requestExpand);
    connect(m_quickPanel, &QuickPanelWidget::iconClicked, this, [this] {
        if (m_model->state() == WirelessCastingModel::CastingState::Connected && !m_model->multiscreensFlag()) {
            m_model->disconnectMonitor();
        } else {
            Q_EMIT requestExpand();
        }
    });
    connect(m_appletWidget, &WirelessCastingApplet::requestHideApplet, this, &WirelessCastingItem::requestHideApplet);
    m_appletWidget->installEventFilter(this);
}

QLabel *WirelessCastingItem::tips()
{
    if (!m_tips) {
        m_tips = new QLabel;
        m_tips->setForegroundRole(QPalette::BrightText);
        m_tips->setContentsMargins(0, 0, 0, 0);
        //connect(qApp, &QGuiApplication::fontChanged, this, &WirelessCastingItem::tips);
    }

    if (m_model->state() == WirelessCastingModel::Connected && m_displayMode->screens().size() > 1) {
        m_tips->setText(tr("Multiple services started"));
    } else if (m_model->state() != WirelessCastingModel::Connected && m_displayMode->screens().size() > 1) {
        m_tips->setText(m_displayMode->currentMode());
    } else if (WirelessCastingModel::Connected != m_model->state()) {
        if (m_model->connectState()) {
            m_tips->setText(tr("Connecting"));
        } else {
            m_tips->setText(tr("Not connected"));
        }
    } else {
        m_tips->setText(m_model->curMonitorName());
    }
    m_tips->adjustSize();
    return m_tips;
}

WirelessCastingItem::~WirelessCastingItem()
{
    delete m_quickPanel;
    delete m_trayIcon;
    if (m_tips) {
        m_tips->deleteLater();
        m_tips = nullptr;
    }
}

QWidget *WirelessCastingItem::quickPanelWidget() const
{
    return m_quickPanel;
}

QWidget *WirelessCastingItem::trayIcon()
{
    return this;
}

QWidget *WirelessCastingItem::appletWidget() const
{
    return m_appletWidget;
}

QWidget *WirelessCastingItem::refreshButton() const
{
#ifdef WIRELESS_CASTING_ENABLED
    if (m_appletWidget)
        return m_appletWidget->refreshButton();
#endif
    return nullptr;
}

void WirelessCastingItem::setAppletMinHeight(int minHeight)
{
    m_appletWidget->setMinHeight(minHeight);
}

void WirelessCastingItem::resizeEvent(QResizeEvent *event)
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
}

void WirelessCastingItem::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    m_trayIcon->paint(&painter, rect());
}

void WirelessCastingItem::init()
{
    m_quickPanel->setActive(true);
    m_quickPanel->setText(tr("Multiple Displays"));
    m_quickPanel->setDescription(tr("Not connected"));
    m_quickPanel->setIcon(QIcon::fromTheme("network-display-failed-symbolic"));

    *m_trayIcon = QIcon::fromTheme("network-display-succeed-symbolic");
    setForegroundRole(QPalette::BrightText);
}

bool WirelessCastingItem::eventFilter(QObject *watcher, QEvent *event)
{
    if (watcher == m_appletWidget) {
        if (event->type() == QEvent::Hide) {
            m_model->enableRefresh(false);
        } else if (event->type() == QEvent::Show) {
            m_model->enableRefresh(true);
            m_appletWidget->resizeApplet();
        }
    }
    return QObject::eventFilter(watcher, event);
}

const QString WirelessCastingItem::contextMenu() const
{
    QList<QVariant> items;
    items.reserve(1);

    QMap<QString, QVariant> settings;
    settings["itemId"] = SETTINGS;
    settings["itemText"] = tr("Display settings");
    settings["isActive"] = true;
    items.push_back(settings);

    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;
    return QJsonDocument::fromVariant(menu).toJson();
}

void WirelessCastingItem::invokeMenuItem(const QString menuId, const bool checked)
{
    Q_UNUSED(checked);
    if (menuId == SETTINGS) {
        QStringList args {"--by-user", "org.deepin.dde.control-center", "--", "-p", "display"};
        QProcess::startDetached("dde-am", args);

        Q_EMIT requestHideApplet();
    }
}

} // namespace wirelesscasting
} // namespace dde
