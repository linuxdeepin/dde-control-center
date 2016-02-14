/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIRELESSPLUGIN_H
#define WIRELESSPLUGIN_H

#include <QObject>
#include <QSettings>
#include <QPointer>

#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>

#include <libdui/dinputdialog.h>

#include "dbus/dbusnetwork.h"
#include "wirelessitem.h"

DUI_USE_NAMESPACE

class QLabel;
class WirelessPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-network-wireless-plugin.json")
    Q_INTERFACES(DockPluginInterface)

public:
    WirelessPlugin();
    ~WirelessPlugin() Q_DECL_OVERRIDE;

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    QPixmap getIcon(QString) Q_DECL_OVERRIDE;
    QWidget *getItem(QString id) Q_DECL_OVERRIDE;
    QWidget *getApplet(QString id) Q_DECL_OVERRIDE;
    bool configurable(const QString &id) Q_DECL_OVERRIDE;
    bool enabled(const QString &id) Q_DECL_OVERRIDE;
    void setEnabled(const QString &id, bool enabled) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString, QString, bool) Q_DECL_OVERRIDE;

private:
    QSettings *m_settings;
    DockPluginProxyInterface *m_proxy;
    QMap<QString, WirelessItem *> m_itemMap;
    QStringList m_uuids;
    Dock::DockMode m_mode = Dock::EfficientMode;
    DBusNetwork *m_dbusNetwork;
    QPointer<DInputDialog> m_passworkInputDialog;
    QString m_targetConnectPath;
    QString m_tragetConnectUuid;

    void initSettings();
    void updateUuids();
    void addNewItem(const QString &id);
    void removeItem(const QString &id);
    void onEnabledChanged(const QString &id);
    void onDevicesChanged();

    Q_SLOT void onNeedSecrets(const QString &path, const QString &uuid, const QString &ssid, bool in3);

    QString settingEnabledKey(const QString &id);
};

#endif // WIRELESSPLUGIN_H
