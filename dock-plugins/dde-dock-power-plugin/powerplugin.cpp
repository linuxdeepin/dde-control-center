/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QIcon>
#include <QFile>
#include <QTimer>
#include <QDBusConnection>

#include "powerplugin.h"

const QString POWER_PLUGIN_ID = "power_plugin_id";

enum MenuItemType{
    SeparatorHorizontal = -1,
    CustomMenuItem = 0,
    PowerSaverMenuItem,
    BalancedMenuItem,
    HighPerformanceMenuItem
};

PowerPlugin::PowerPlugin()
{
    QIcon::setThemeName("deepin");

    m_label = new QLabel;
    m_label->adjustSize();

    m_dbusPower = new com::deepin::daemon::DBusPower(this);
    connect(m_dbusPower, &DBusPower::BatteryPercentageChanged, this, &PowerPlugin::updateIcon);
    connect(m_dbusPower, &DBusPower::OnBatteryChanged, this, &PowerPlugin::updateIcon);

    initSettings();
}

PowerPlugin::~PowerPlugin()
{

}

void PowerPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    m_mode = proxy->dockMode();

    //for init
    onEnabledChanged();
}

QString PowerPlugin::getPluginName()
{
    return tr("Power");
}

QStringList PowerPlugin::ids()
{
    return QStringList(POWER_PLUGIN_ID);
}

QString PowerPlugin::getName(QString)
{
    return getPluginName();
}

int PowerPlugin::getBatteryPercentage() {
    BatteryPercentageMap pertMap = m_dbusPower->batteryPercentage();
    int totalBatPercentage = 0;
    BatteryPercentageMap::const_iterator i = pertMap.constBegin();
    while (i != pertMap.constEnd()) {

       totalBatPercentage += pertMap.value(i.key());
       i++;
    }
    return totalBatPercentage;
}

bool PowerPlugin::getBatteryIsPresent() {
    BatteryInfoMap infoMap = m_dbusPower->batteryIsPresent();
    bool isPresent = false;
    BatteryInfoMap::const_iterator j = infoMap.constBegin();
    while( j != infoMap.constEnd()) {
        isPresent = (infoMap.value(j.key()) || isPresent);
        j++;
    }
    return isPresent;
}

QString PowerPlugin::getTitle(QString)
{
    if (!m_dbusPower->isValid())
        return getPluginName();


    QString batteryPercentage = QString("%1%").arg(QString::number(int(getBatteryPercentage())));

    if (!m_dbusPower->onBattery()) {
        return tr("On Charging %1").arg(batteryPercentage);
    } else {
        return batteryPercentage;
    }
}

QString PowerPlugin::getCommand(QString)
{
    return "dde-control-center power";
}

QPixmap PowerPlugin::getIcon(QString)
{
    QString iconName = getBatteryIcon(100, false, true);
    QIcon icon = QIcon::fromTheme(iconName, QIcon::fromTheme("application-default-icon"));
    return icon.pixmap(QSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE));
}

int callDBusRetryCount = 10;
bool PowerPlugin::configurable(const QString &)
{
    if(m_dbusPower->isValid())
        return getBatteryIsPresent();

    if(!m_detectionDBusTimer) {
        m_detectionDBusTimer = new QTimer(this);

        connect(m_detectionDBusTimer, &QTimer::timeout, this, [this] {
            if(m_dbusPower->isValid()) {
                m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, POWER_PLUGIN_ID);

                onEnabledChanged();
                m_detectionDBusTimer->deleteLater();
                m_detectionDBusTimer = NULL;
                callDBusRetryCount = 10;
                return;
            }

            if(--callDBusRetryCount < 0) {
                m_detectionDBusTimer->deleteLater();
                m_detectionDBusTimer = NULL;
                return;
            }
            m_detectionDBusTimer->start(1500);
        });
    }

    m_detectionDBusTimer->start(1500);

    return false;
}

bool PowerPlugin::enabled(const QString &)
{
    return m_settings->value(settingEnabledKey(), true).toBool();
}

void PowerPlugin::setEnabled(const QString & id, bool enable)
{
    if (id != POWER_PLUGIN_ID)
        return;

    m_settings->setValue(settingEnabledKey(), enable);

    onEnabledChanged();
}

QWidget * PowerPlugin::getApplet(QString)
{
    return NULL;
}

QWidget * PowerPlugin::getItem(QString)
{
    if (!m_dbusPower->isValid())
        return NULL;

    if (getBatteryIsPresent() && enabled(POWER_PLUGIN_ID)) {
        return m_label;
    } else {
        return NULL;
    }
}

void PowerPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    if (m_dbusPower->isValid() && (newMode != oldMode)){
        //make sure m_mode changed beford update enable-state
        setMode(newMode);
        onEnabledChanged();
    }
}

QJsonObject createMenuItem(MenuItemType itemId, QString itemName, bool checked = false, bool checkable = true)
{
    QJsonObject itemObj;

    itemObj.insert("itemId", QString::number(itemId));
    itemObj.insert("itemText", itemName);
    itemObj.insert("itemIcon", "");
    itemObj.insert("itemIconHover", "");
    itemObj.insert("itemIconInactive", "");
    itemObj.insert("itemExtra", "");
    itemObj.insert("isActive", true);
    itemObj.insert("isCheckable", checkable);
    itemObj.insert("checked", checked);
    itemObj.insert("itemSubMenu", QJsonObject());

    return itemObj;
}

QString PowerPlugin::getMenuContent(QString)
{
//    QJsonObject contentObj;

//    QJsonArray items;

//    MenuItemType type;
//    if(m_dbusPower->onBattery()){
//        type = (MenuItemType)m_dbusPower->batteryPlan();
//    }else{
//        type = (MenuItemType)m_dbusPower->linePowerPlan();
//    }

//    items.append(createMenuItem(SeparatorHorizontal, ""));
//    items.append(createMenuItem(CustomMenuItem, tr("Custom"), type == CustomMenuItem));
//    items.append(createMenuItem(PowerSaverMenuItem, tr("Power saver"), type == PowerSaverMenuItem));
//    items.append(createMenuItem(BalancedMenuItem, tr("Balanced"), type == BalancedMenuItem));
//    items.append(createMenuItem(HighPerformanceMenuItem, tr("High performance"), type == HighPerformanceMenuItem));
//    items.append(createMenuItem(SeparatorHorizontal, ""));

//    contentObj.insert("items", items);

//    return QString(QJsonDocument(contentObj).toJson());
    return QString("");
}

void PowerPlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{
    Q_UNUSED(id)
    Q_UNUSED(itemId);
    if(checked){
//        if(m_dbusPower->onBattery())
//            m_dbusPower->setBatteryPlan(itemId.toInt());
//        else
//            m_dbusPower->setLinePowerPlan(itemId.toInt());
    }
}

// private methods
void PowerPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-power-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        m_settings->setValue(settingEnabledKey(), true);
    }
}

//int retryTimes = 10;
void PowerPlugin::onEnabledChanged()
{
    if (!m_dbusPower->isValid() /*&& retryTimes-- > 0*/) {
//        QTimer *retryTimer = new QTimer;
//        retryTimer->setSingleShot(true);
//        connect(retryTimer, &QTimer::timeout, this, &PowerPlugin::onEnabledChanged);
//        connect(retryTimer, &QTimer::timeout, retryTimer, &QTimer::deleteLater);
//        retryTimer->start(1000);
        qWarning() << "[PowerManagerPlugin] PowerManager dbus data is not ready!";
        return;
    }
//    retryTimes = 10;

    m_proxy->itemRemovedEvent(POWER_PLUGIN_ID);
    m_label->setParent(NULL);

    if (enabled(POWER_PLUGIN_ID)) {
        m_proxy->itemAddedEvent(POWER_PLUGIN_ID);
        updateIcon();
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, POWER_PLUGIN_ID);
}

void PowerPlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    updateIcon();
}

QString PowerPlugin::settingEnabledKey()
{
    return QString::number(m_mode) + "/enabled";
}

void PowerPlugin::updateIcon()
{
    if (m_label == nullptr)
        return;

    QString iconName;

    if (!m_dbusPower->isValid()){
        iconName = getBatteryIcon(-1, false, m_mode != Dock::FashionMode);
    }else{
        bool batteryPresent =  getBatteryIsPresent();

        if (batteryPresent) {
            int batteryPercentage = getBatteryPercentage();
            iconName = getBatteryIcon(batteryPercentage, !m_dbusPower->onBattery(),
                                      m_mode != Dock::FashionMode);
        }
    }

    if (m_mode == Dock::FashionMode) {
        m_label->setFixedSize(Dock::APPLET_FASHION_ICON_SIZE, Dock::APPLET_FASHION_ICON_SIZE);
    } else {
        m_label->setFixedSize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);
    }

    QIcon fallback = QIcon::fromTheme("application-default-icon");
    QIcon icon = QIcon::fromTheme(iconName, fallback);
    m_label->setPixmap(icon.pixmap(m_label->size()));
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeItemSize, POWER_PLUGIN_ID);
}

QString PowerPlugin::getBatteryIcon(int percentage, bool plugged, bool symbolic)
{
    QString percentageStr;

    if (percentage < 10 && percentage >= 0) {
        percentageStr = "000";
    } else if (percentage < 30) {
        percentageStr = "020";
    } else if (percentage < 50) {
        percentageStr = "040";
    } else if (percentage < 70) {
        percentageStr = "060";
    } else if (percentage < 90) {
        percentageStr = "080";
    } else if (percentage <= 100){
        percentageStr = "100";
    } else {
        if(!symbolic)
            return "battery-unknow";
        else
            percentageStr = "000";
    }

    if (symbolic) {
        if (plugged) {
            return "battery-charged-symbolic";
        } else {
            return QString("battery-%1-symbolic").arg(percentageStr);
        }
    } else {
        if (plugged) {
            if(percentage < 100)
                return QString("battery-%1-plugged").arg(percentageStr);
            else
                return "battery-full-charged";
        } else {
            return QString("battery-%1").arg(percentageStr);
        }
    }
}
