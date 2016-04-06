/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QFrame>
#include <QDBusConnection>
#include <QMessageBox>

#include "moduleheader.h"
#include "listwidget.h"

#include "display.h"
#include "customsettings.h"
#include "monitorground.h"
#include "monitor.h"
#include "displaymodeitem.h"
#include "scrollframe.h"
#include "constants.h"

Display::Display():
    QObject(),
    m_frame(NULL)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Display"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    init();
}

Display::~Display()
{
    qDebug() << "~Display()";
    delete m_frame ;
}

QFrame *Display::getContent()
{
    return m_frame;
}

void Display::init()
{
    m_dbusDisplay = new DisplayInterface(this);

    m_frame = new ScrollFrame;
    m_frame->setFixedWidth(DCC::ModuleContentWidth);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_frame->mainLayout()->setMargin(0);
    m_frame->mainLayout()->setSpacing(0);

    ModuleHeader * headerLine = new ModuleHeader(tr("Display"));

    headerLine->setFixedWidth(DCC::ModuleContentWidth);
    connect(headerLine, &ModuleHeader::resetButtonClicked, m_dbusDisplay, &DisplayInterface::Reset);

    m_monitorGround = new MonitorGround(m_dbusDisplay);

    m_frame->headerLayout()->addWidget(headerLine);
    m_frame->mainLayout()->addWidget(new DSeparatorHorizontal);
    m_frame->mainLayout()->addWidget(m_monitorGround);
    m_frame->mainLayout()->addWidget(new DSeparatorHorizontal);

    m_singleSettings = new CustomSettings(m_dbusDisplay, m_monitorGround, m_dbusMonitors);
    m_singleSettings->setFixedWidth(DCC::ModuleContentWidth);
    m_singleSettings->hide();

#ifndef ARCH_MIPSEL
    updateUI();
#else
    QTimer::singleShot(100, this, &Display::updateUI);
#endif

    connect(m_dbusDisplay, &DisplayInterface::MonitorsChanged,
            this, &Display::updateUI, Qt::DirectConnection);
    connect(m_dbusDisplay, &DisplayInterface::DisplayModeChanged,
            this, &Display::onDisplayModeChanged, Qt::DirectConnection);
    connect(m_dbusDisplay, &DisplayInterface::PrimaryChanged, this, [this]{
        QString str = m_dbusDisplay->primary();

        for(int i=0; i<m_monitors.count(); ++i){
            m_monitors[i]->setIsPrimary(m_monitors[i]->name() == str);
        }
    }, Qt::DirectConnection);
}

void Display::updateUI()
{
    QList<QDBusObjectPath> pathList = m_dbusDisplay->monitors();

    m_monitorGround->clear();

    qDeleteAll(m_monitors);
    qDeleteAll(m_dbusMonitors);

    m_monitors.clear();
    m_dbusMonitors.clear();

    QDBusPendingReply<QString> reply_tmp = m_dbusDisplay->QueryCurrentPlanName();
    reply_tmp.waitForFinished();
    QStringList tmp_list = reply_tmp.value().split(",");

    QString primargName = m_dbusDisplay->primary();

    for (int i=0; i<pathList.count(); ++i) {
        MonitorInterface *interface = new MonitorInterface(pathList[i].path(), this);
        m_dbusMonitors << interface;
        Monitor *monitor = new Monitor(interface);

        if(tmp_list.count() == pathList.count())
            monitor->setName(interface->name());
        else if(i < tmp_list.count() - 1)
            monitor->setName(tmp_list[i]);

        if(monitor->name() == primargName)
            monitor->setIsPrimary(true);

        if(interface->isComposited()){
            Monitor *tmp = new Monitor(NULL);
            tmp->setName(tmp_list[1]);
            monitor->drop(tmp);
        }

        m_monitors << monitor;
        m_monitorGround->addMonitor(monitor);
    }

    m_singleSettings->updateUI(m_dbusMonitors);

    if(m_monitorNameList == tmp_list)
        return;

    m_monitorNameList = tmp_list;

    for(int i = 3; i < m_frame->mainLayout()->count();){
        QLayoutItem *item = m_frame->mainLayout()->takeAt(i);
        QWidget *widget = item->widget();
        delete item;

        if(widget == m_singleSettings){
            widget->hide();
        }else{
            widget->deleteLater();
        }
    }

    if(m_monitorNameList.count() > 1){
        DArrowLineExpand *displayModeExpand = new DArrowLineExpand;
        displayModeExpand->setTitle(tr("Display Mode"));

        m_widgetList = new ListWidget;
        m_widgetList->setItemSize(DCC::ModuleContentWidth, 90);

        DisplayModeItem *item_copy = new DisplayModeItem;
        item_copy->setTitle(tr("Copy"));
        item_copy->setText(tr("Copy the contents of your primary screen to other screens."));
        item_copy->setIconName("copy");
        connect(item_copy, &DisplayModeItem::checkedChanged,
                this, [this](bool arg){
            if(arg && m_dbusDisplay->displayMode() != 1){
                m_dbusDisplay->SwitchMode(1, "");
            }
        }, Qt::DirectConnection);

        DisplayModeItem *item_extend = new DisplayModeItem;
        item_extend->setTitle(tr("Extend"));
        item_extend->setText(tr("Extend your screen contents to display different contents on different screens."));
        item_extend->setIconName("extend");

        connect(item_extend, &DisplayModeItem::checkedChanged,
                this, [this](bool arg){
            if(arg && m_dbusDisplay->displayMode() != 2){
                m_dbusDisplay->SwitchMode(2, "");
            }
        }, Qt::DirectConnection);

        DisplayModeItem *item_settings = new DisplayModeItem(false, true, true);
        item_settings->setTitle(tr("Custom Settings"));
        item_settings->setText(tr("You can do other custom settings to your screens."));
        item_settings->setIconName("customize");

        m_widgetList->addWidget(item_copy);
        m_widgetList->addWidget(item_extend);

        for (int i=0; i < m_monitorNameList.count(); ++i) {
            const QString& name = m_monitorNameList[i];

            DisplayModeItem *item_monitor = new DisplayModeItem;
            item_monitor->setTitle(tr("Only Displayed on %1").arg(name));
            item_monitor->setText(tr("Screen contents are only displayed on %1 but not on other screens.").arg(name));
            item_monitor->setIconName("single");
            item_monitor->setIconText(QString::number(i + 1));
            m_widgetList->addWidget(item_monitor);

            connect(item_monitor, &DisplayModeItem::checkedChanged,
                    this, [this, name](bool arg){
                if(arg){
                    m_dbusDisplay->SwitchMode(3, name);
                }
            }, Qt::DirectConnection);
        }

        m_widgetList->addWidget(item_settings);

        displayModeExpand->setContent(m_widgetList);

        connect(item_settings, &DisplayModeItem::clicked, displayModeExpand, [this, displayModeExpand]{
            m_dbusDisplay->SwitchMode(0, "");
        }, Qt::DirectConnection);

        connect(item_settings, &DisplayModeItem::rightArrowClicked, [displayModeExpand, this] {
            displayModeExpand->hide();
            m_singleSettings->show();
            m_monitorGround->setEditable(true);
        });

        connect(m_singleSettings, &CustomSettings::cancel, displayModeExpand, [this, displayModeExpand]{
            displayModeExpand->show();
            m_singleSettings->hide();
            onDisplayModeChanged();
            m_monitorGround->setEditable(false);
        });

        m_frame->mainLayout()->addWidget(displayModeExpand);
        m_frame->mainLayout()->addWidget(m_singleSettings);

        onDisplayModeChanged();

        displayModeExpand->setExpand(true);
    }else{
        m_frame->mainLayout()->addWidget(m_singleSettings);
        m_singleSettings->show();
    }

    m_frame->mainLayout()->addStretch(1);
}

void Display::onDisplayModeChanged()
{
    if(!m_widgetList)
        return;

    switch (m_dbusDisplay->displayMode()) {
    case 0://custom mode
        m_widgetList->getWidget(m_monitorNameList.count() + 2)->setProperty("checked", true);
        break;
    case 1:{//copy mode
        m_widgetList->getWidget(0)->setProperty("checked", true);
        break;
    }
    case 2:{
        m_widgetList->getWidget(1)->setProperty("checked", true);
        break;
    }
    case 3:{
        QString str = m_dbusDisplay->primary();

        for(int i = 2; i<m_widgetList->count() - 1; ++i){
            DisplayModeItem *item = qobject_cast<DisplayModeItem*>(m_widgetList->getWidget(i));
            if(item && item->title().contains(str)){
                m_widgetList->getWidget(i)->setProperty("checked", true);
            }
        }
        break;
    }
    default:
        break;
    }
}


