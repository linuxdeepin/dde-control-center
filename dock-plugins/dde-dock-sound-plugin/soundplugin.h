/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include <QMap>
#include <QLabel>
#include <QUuid>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QDebug>
#include <QSettings>
#include <dde-dock/dockconstants.h>
#include <dde-dock/dockplugininterface.h>
#include <dde-dock/dockpluginproxyinterface.h>
#include "mainitem.h"
#include "soundcontent.h"


class SoundPlugin : public QObject, public DockPluginInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.deepin.Dock.PluginInterface" FILE "dde-dock-sound-plugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(DockPluginInterface)


public:
    SoundPlugin();
    ~SoundPlugin();

    void init(DockPluginProxyInterface *proxy) Q_DECL_OVERRIDE;

    QString getPluginName() Q_DECL_OVERRIDE;

    QStringList ids() Q_DECL_OVERRIDE;
    QString getName(QString id) Q_DECL_OVERRIDE;
    QString getTitle(QString id) Q_DECL_OVERRIDE;
    QString getCommand(QString id) Q_DECL_OVERRIDE;
    QPixmap getIcon(QString) Q_DECL_OVERRIDE;
    bool configurable(const QString &id) Q_DECL_OVERRIDE;
    bool enabled(const QString &id) Q_DECL_OVERRIDE;
    void setEnabled(const QString &id, bool enabled) Q_DECL_OVERRIDE;
    QWidget * getItem(QString id) Q_DECL_OVERRIDE;
    QWidget * getApplet(QString id) Q_DECL_OVERRIDE;
    void changeMode(Dock::DockMode newMode, Dock::DockMode oldMode) Q_DECL_OVERRIDE;

    QString getMenuContent(QString id) Q_DECL_OVERRIDE;
    void invokeMenuItem(QString id, QString itemId, bool checked) Q_DECL_OVERRIDE;

private slots:
    void onInitTimerTriggered();

private:
    QString m_id = "sound_plugin";
    DockPluginProxyInterface * m_proxy;

    Dock::DockMode m_mode = Dock::FashionMode;
    MainItem * m_item = NULL;
    SoundContent *m_soundContent = NULL;
    QSettings * m_settings = NULL;
    DBusAudio *m_audio = NULL;

    void initSettings();
    void onEnabledChanged();
    void setMode(Dock::DockMode mode);
    QString settingEnabledKey();
    QJsonObject createMenuItem(QString itemId,
                               QString itemName,
                               bool checkable = false,
                               bool checked = false);

};

#endif // SOUNDPLUGIN_H
