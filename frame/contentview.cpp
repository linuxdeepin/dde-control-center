/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QPluginLoader>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>

#include "contentview.h"
#include "sidebar.h"
#include "constants.h"
#include "controlcenterproxy.h"

ContentView::ContentView(ControlCenterProxy *proxy, QWidget *parent)
    : QFrame(parent),
    m_controlCenterProxy(proxy)
{
    m_pluginLoader = new QPluginLoader(this);
#ifdef QT_DEBUG
    m_pluginLoader->setLoadHints(QLibrary::ResolveAllSymbolsHint);
#endif
    m_pluginsManager = PluginsManager::getInstance(this);

    m_sideBar = new SideBar(parent);

    m_leftSeparator = new DSeparatorVertical;
    m_rightSeparator = new DSeparatorVertical;

    m_lastPluginWidgetContainer = new QWidget;
    m_lastPluginWidgetContainerLayout = new QHBoxLayout(m_lastPluginWidgetContainer);

    m_lastPluginWidgetContainerLayout->setMargin(0);
    m_lastPluginWidgetContainerLayout->setSpacing(0);

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_leftSeparator);
    m_layout->addWidget(m_sideBar);
    m_layout->addWidget(m_rightSeparator);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    if(m_hideInLeft) {
        m_rightSeparator->hide();
//        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowLeft);
        m_layout->insertWidget(0, m_lastPluginWidgetContainer);
    } else {
        m_leftSeparator->hide();
//        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowRight);
        m_layout->addWidget(m_lastPluginWidgetContainer);
    }

    QFrame::hide();
    m_opacityEffect = new QGraphicsOpacityEffect;
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);

    m_showAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_showAni->setDuration(DCC::CommonAnimationDuration);

    m_hideAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_hideAni->setDuration(DCC::CommonAnimationDuration);

    connect(m_sideBar, &SideBar::moduleSelected, this, &ContentView::onModuleSelected);
    connect(m_hideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
    connect(m_showAni, &QPropertyAnimation::finished, this, static_cast<void (ContentView::*)()>(&ContentView::update));
    connect(m_pluginsManager, &PluginsManager::showHome, this,&ContentView::switchToHome );

    setLayout(m_layout);
}

ContentView::~ContentView()
{
    m_pluginLoader->deleteLater();
    m_opacityEffect->deleteLater();
    m_showAni->deleteLater();
    m_hideAni->deleteLater();
}

void ContentView::switchToModule(ModuleMetaData module)
{
    if (m_lastPluginInterface && m_lastPluginWidget && m_lastPluginPath == module.path)
        return;
    m_lastPluginPath = module.path;

    unloadPlugin();

    qDebug() << "load plugin: " << module.path;

    // load new plugin
    m_sideBar->blockSignals(true);
    m_sideBar->switchToModule(module);
    m_sideBar->blockSignals(false);

    QWidget * content = nullptr;
#ifdef DCC_CACHE_MODULES
    if (m_pluginsCache.contains(module.path)) {
        content = m_pluginsCache.value(module.path, nullptr);
    } else {
        content = loadPlugin(module);
        m_pluginsCache[module.path] = content;
    }
#else
    content = loadPlugin(module);
#endif

    m_lastPluginWidget = content;
    m_lastPluginWidget->setFixedWidth(DCC::ModuleContentWidth);
    m_lastPluginWidgetContainerLayout->addWidget(m_lastPluginWidget);
    m_lastPluginWidget->show();
}

void ContentView::hide()
{
    m_showAni->stop();
    m_hideAni->stop();
    m_hideAni->setStartValue(1.0);
    m_hideAni->setEndValue(0.0);
    m_hideAni->start();
}

void ContentView::show()
{
    m_hideAni->stop();
    m_showAni->stop();
    m_showAni->setStartValue(0.0);
    m_showAni->setEndValue(1.0);
    m_showAni->start();

    QFrame::show();
}

void ContentView::reLayout(bool hideInLeft)
{
    if(hideInLeft == m_hideInLeft)
        return;

    m_hideInLeft = hideInLeft;

    m_rightSeparator->setHidden(hideInLeft);
    m_leftSeparator->setVisible(hideInLeft);
    int index = hideInLeft ? 3 : 0;
    if(m_layout->count() > 3){
        QLayoutItem *item = m_layout->takeAt(index);

        if(item){
            m_layout->insertItem(3 - index, item);
        }
    }
}

void ContentView::switchToHome()
{
    emit backToHome();
}

void ContentView::switchToModule(const QString pluginId)
{
    switchToModule(m_pluginsManager->pluginMetaData(pluginId));
}

QWidget * ContentView::loadPlugin(ModuleMetaData module)
{
#ifdef DCC_CACHE_MODULES
    QPluginLoader * pluginLoader = new QPluginLoader(this);
    pluginLoader->setFileName(module.path);
    QObject *instance = pluginLoader->instance();
#else
    m_pluginLoader->setFileName(module.path);
    QObject *instance = m_pluginLoader->instance();
#endif

    ModuleInterface *interface = qobject_cast<ModuleInterface *>(instance);
    qDebug() << "get instance: " << instance << interface;

    QWidget *content = nullptr;

    do {

        if (!interface)
            break;

        m_lastPluginInterface = interface;

        content = interface->getContent();

        if (!content) {
            // display error infomation
            const QString error = m_pluginLoader->errorString();
            // this label will destory when call unloadPlugin() next time
            QLabel *errorLabel = new QLabel(error);
            errorLabel->setWordWrap(true);
            errorLabel->setStyleSheet("color:red;");
            content = errorLabel;
            break;
        }

        m_lastPluginInterface->setProxy(m_controlCenterProxy);

    } while (false);

    return content;
}

void ContentView::onModuleSelected(ModuleMetaData meta)
{
    qDebug() << meta.id;

    if (meta.id == "home")
    {
        // when goto home screen, notify plugin know.
        if (m_lastPluginInterface)
            m_lastPluginInterface->preUnload();

        emit backToHome();
        return;
    }

    if (meta.id == "shutdown")
    {
        emit shutdownSelected();
        return;
    }

    // switch to another plugin
    return switchToModule(meta);
}

void ContentView::unloadPlugin()
{
    if (m_lastPluginInterface)
        m_lastPluginInterface->preUnload();

    if (m_lastPluginWidget)
    {
        m_lastPluginWidget->hide();
        m_lastPluginWidget->setParent(nullptr);
#ifndef DCC_CACHE_MODULES
        m_lastPluginWidget->deleteLater();
#endif
//        delete m_lastPluginWidget;
        m_lastPluginWidget = nullptr;
    }

    if (m_lastPluginInterface)
    {
//        m_lastPluginInterface->preUnload();
#ifndef DCC_CACHE_MODULES
        delete m_lastPluginInterface;
#endif
        m_lastPluginInterface = nullptr;
    }
}
