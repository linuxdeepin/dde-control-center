// SPDX-FileCopyrightText: 2020 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "brightnessitem.h"
#include "constants.h"
#include "brightnessmodel.h"


#include <DGuiApplicationHelper>

#include <QDBusConnection>
#include <QIcon>
#include <QJsonDocument>
#include <QPainter>
#include <QProcess>
#include <QVBoxLayout>

DGUI_USE_NAMESPACE

#define SETTINGS "settings"

BrightnessItem::BrightnessItem(QObject* parent)
    : QObject(parent)
    , m_tipsLabel(nullptr)
    , m_applet(new BrightnessApplet)
    , m_icon(new CommonIconButton)
{
    init();
}

BrightnessItem::~BrightnessItem()
{
    if (m_tipsLabel) {
        m_tipsLabel->deleteLater();
        m_tipsLabel = nullptr;
    }

    if (m_applet) {
        m_applet->deleteLater();
        m_applet = nullptr;
    }

    if (m_icon) {
        m_icon->deleteLater();
        m_icon = nullptr;
    }
}

void BrightnessItem::init()
{
    m_icon->setFixedSize(Dock::DOCK_PLUGIN_ITEM_FIXED_SIZE);
    m_icon->setIcon(QIcon::fromTheme("display-brightness-control"));
    connect(m_applet, &BrightnessApplet::requestHideApplet, this, &BrightnessItem::requestHideApplet);
}

const QString BrightnessItem::contextMenu() const
{
    QList<QVariant> items;
    items.reserve(2);

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

void BrightnessItem::invokeMenuItem(const QString menuId, const bool checked)
{
    Q_UNUSED(menuId);
    Q_UNUSED(checked);

    if (menuId == SETTINGS) {
        QStringList args {"--by-user", "org.deepin.dde.control-center", "--", "-p", "display"};
        QProcess::startDetached("dde-am", args);

        Q_EMIT requestHideApplet();
    }
}

QWidget *BrightnessItem::tipsWidget()
{
    updateTips();
    return m_tipsLabel;
}

void BrightnessItem::updateTips()
{
    if (!m_tipsLabel) {
        m_tipsLabel = new QLabel;
        m_tipsLabel->setForegroundRole(QPalette::BrightText);
        m_tipsLabel->setContentsMargins(0, 0, 0, 0);
        connect(&BrightnessModel::ref(), &BrightnessModel::enabledMonitorListChanged, this, &BrightnessItem::updateTips);
        connect(&BrightnessModel::ref(), &BrightnessModel::monitorBrightnessChanged, this, &BrightnessItem::updateTips);
    }

    QString tips;
    auto monitors = BrightnessModel::ref().enabledMonitors();
    for (const auto &monitor : monitors) {
        tips += QString("%1: %2%<br/>").arg(monitor->name()).arg(QString::number(monitor->brightness() * 100));
    }

    // remove HTML default margin
    tips = "<p style='margin:0;'>" + tips + "</p>";
    m_tipsLabel->setText(tips);

    QFontMetrics fm(m_tipsLabel->font());
    int lineHeight = fm.height();
    int textHeight = lineHeight * (int)monitors.count();
    m_tipsLabel->setFixedHeight(textHeight);
}
