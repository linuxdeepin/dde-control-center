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
    void loadPluginInstance(const QString &, QObject *);
    QWidget *loadPlugin(ModuleMetaData module);
    QWidget *loadPluginNow(ModuleMetaData module);
    QWidget *loadModuleContent();
    void unloadPlugin();
    void switchToHome();

signals:
    void backToHome();
    void shutdownSelected();

private:
    QHBoxLayout *m_layout;
    SideBar *m_sideBar;
    QPluginLoader *m_pluginLoader;
    ControlCenterProxy *m_controlCenterProxy;
#ifdef DCC_CACHE_MODULES
    QMap<QString, QWidget *> m_pluginsCache;
    QMap<QString, QObject *> m_moduleCache;
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
    QQueue<QPair<ModuleMetaData, ModuleInterface *> > m_moduleLoadQueue;

    QWidget *m_lastPluginWidgetContainer;
    QHBoxLayout *m_lastPluginWidgetContainerLayout;
    QString m_lastPluginPath;

    bool m_hideInLeft = false;
};

class PluginLoader : public QObject
{
    Q_OBJECT
public:
    explicit PluginLoader(QObject *parent = 0): QObject(parent) {;}
    QList<ModuleMetaData> list;

signals:
    void workFinished();
    void pluginLoad(const QString &, QObject *);

public slots:
    void runLoader();

};

#endif // CONTENTVIEW_H
