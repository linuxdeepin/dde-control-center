/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QFrame>
#include <QPluginLoader>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include <dseparatorvertical.h>
#include <dwidget_global.h>

#include "modulemetadata.h"
#include "pluginsmanager.h"
#include "interfaces.h"

DWIDGET_USE_NAMESPACE

class ControlCenterProxy;
class QHBoxLayout;
class SideBar;
class ContentView : public QFrame
{
    Q_OBJECT
public:
    explicit ContentView(ControlCenterProxy *proxy, QWidget *parent = 0);
    ~ContentView();

    void switchToModule(ModuleMetaData module);
    void hide();
    void show();

public slots:
    void reLayout(bool hideInLeft);
    void switchToModule(const QString pluginId);
    QWidget * loadPlugin(ModuleMetaData module);
    void unloadPlugin();
    void switchToHome();
    void lazyQueueLoadModules();

signals:
    void backToHome();
    void shutdownSelected();

private:
    QHBoxLayout *m_layout;
    SideBar *m_sideBar;
    QPluginLoader *m_pluginLoader;
    ControlCenterProxy *m_controlCenterProxy;
#ifdef DCC_CACHE_MODULES
    QMap<QString, QWidget*> m_pluginsCache;
#endif

private slots:
    void onModuleSelected(ModuleMetaData meta);

private:
    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_showAni;
    QPropertyAnimation *m_hideAni;
    DSeparatorVertical *m_leftSeparator;
    DSeparatorVertical *m_rightSeparator;
    PluginsManager *m_pluginsManager;
    QWidget *m_lastPluginWidget = nullptr;
    ModuleInterface *m_lastPluginInterface = nullptr;
    QWidget *m_lastPluginWidgetContainer;
    QHBoxLayout *m_lastPluginWidgetContainerLayout;
    QString m_lastPluginPath;

    bool m_hideInLeft = false;
};

#endif // CONTENTVIEW_H
