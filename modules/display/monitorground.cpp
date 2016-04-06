/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QHBoxLayout>

#include "imagenamebutton.h"

#include "constants.h"

#include "monitorground.h"
#include "monitor.h"
#include "dbus/monitorinterface.h"
#include "dbus/displayinterface.h"
#include "displaymodeitem.h"
#include "fullscreentooltip.h"

DisplayModeItem * getIconButton(const QString &text, const QString &iconName){
    DisplayModeItem* button = new DisplayModeItem(false, false, NULL);
    button->setText(text);
    button->setClickCheck(false);
    button->setIconName(iconName);
    button->hide();

    return button;
}

MonitorGround::MonitorGround(DisplayInterface * display, QWidget *parent):
    QFrame(parent),
    m_editing(false),
    m_editable(false),
    m_dbusDisplay(display)
{
    setStyleSheet(QString("QFrame { background-color: %1; }").arg(DCC::BgDarkColor.name()));

    setFixedSize(310, 210);

    QHBoxLayout *layout = new QHBoxLayout;

    m_recognize = getIconButton(tr("Recognize"), "recognize");
    m_edit = getIconButton(tr("Edit"), "edit");
    m_split = getIconButton(tr("Split"), "split");

    connect(m_split, &DisplayModeItem::clicked, [this]{
        foreach (Monitor *monitor, m_monitors) {
            MonitorInterface *dbus = monitor->dbusInterface();
            if(dbus->isComposited()){
                m_dbusDisplay->SplitMonitor(dbus->name());
            }
        }

        m_split->hide();
        m_edit->show();
    });

    connect(m_edit, &DisplayModeItem::clicked, this, &MonitorGround::beginEdit);

    layout->addStretch(1);
    layout->addWidget(m_recognize, 0, Qt::AlignBottom);
    layout->addWidget(m_edit, 0, Qt::AlignBottom);
    layout->addWidget(m_split, 0, Qt::AlignBottom);
    layout->addStretch(1);

    setLayout(layout);
}

MonitorGround::~MonitorGround()
{
    qDeleteAll(m_tooltipList.begin(), m_tooltipList.end());
}

void MonitorGround::addMonitor(Monitor *monitor)
{
    if(m_monitors.contains(monitor)||!monitor)
        return;

    m_monitors << monitor;
    monitor->setParent(this);
    MonitorInterface* dbus = monitor->dbusInterface();

    connect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise, Qt::DirectConnection);
    connect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove, Qt::DirectConnection);
    connect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease, Qt::DirectConnection);
//    connect(dbus, &MonitorInterface::XChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
//    connect(dbus, &MonitorInterface::YChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
//    connect(dbus, &MonitorInterface::WidthChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
//    connect(dbus, &MonitorInterface::HeightChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
//    connect(dbus, &MonitorInterface::RotationChanged, this, &MonitorGround::relayout, Qt::DirectConnection);
    connect(monitor, &Monitor::resolutionChanged, this, &MonitorGround::relayout, Qt::UniqueConnection);
    connect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::relayout, Qt::UniqueConnection);
    connect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::updateOpenedCount, Qt::DirectConnection);
    connect(dbus, &MonitorInterface::IsCompositedChanged, this, &MonitorGround::updateOpenedCount, Qt::DirectConnection);
    connect(this, &MonitorGround::applyEdit, monitor, &Monitor::applyPostion);

    updateOpenedCount();
    relayout();

    FullScreenTooltip *tooltip = new FullScreenTooltip(dbus);
    m_tooltipList << tooltip;
    if(m_editing)
        tooltip->showToTopLeft();
    connect(m_recognize, &DisplayModeItem::clicked, tooltip, [this, tooltip]{
        tooltip->showToCenter();
        QTimer::singleShot(3000, tooltip, SLOT(hide()));
    });
}

void MonitorGround::removeMonitor(Monitor *monitor)
{
    int index = m_monitors.indexOf(monitor);
    if(index >= 0){
        FullScreenTooltip *tip = m_tooltipList.at(index);
        m_tooltipList.removeAt(index);
        delete tip;
    }

    m_monitors.removeOne(monitor);

    monitor->setParent(NULL);
    MonitorInterface* dbus = monitor->dbusInterface();

    disconnect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise);
    disconnect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove);
    disconnect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease);
//    disconnect(dbus, &MonitorInterface::XChanged, this, &MonitorGround::relayout);
//    disconnect(dbus, &MonitorInterface::YChanged, this, &MonitorGround::relayout);
//    disconnect(dbus, &MonitorInterface::WidthChanged, this, &MonitorGround::relayout);
//    disconnect(dbus, &MonitorInterface::HeightChanged, this, &MonitorGround::relayout);
//    disconnect(dbus, &MonitorInterface::RotationChanged, this, &MonitorGround::relayout);
    disconnect(monitor, &Monitor::resolutionChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::relayout);
    disconnect(dbus, &MonitorInterface::OpenedChanged, this, &MonitorGround::updateOpenedCount);
    disconnect(dbus, &MonitorInterface::IsCompositedChanged, this, &MonitorGround::updateOpenedCount);
    disconnect(this, &MonitorGround::applyEdit, monitor, &Monitor::applyPostion);

    updateOpenedCount();
    relayout();
}

void MonitorGround::clear()
{
    foreach (Monitor *monitor, m_monitors) {
        removeMonitor(monitor);
    }
}

void MonitorGround::beginEdit()
{
    setEditing(true);

    foreach (Monitor *monitor, m_monitors) {
        monitor->setDraggable(true);
    }

    m_recognize->hide();
    m_edit->hide();

    foreach (FullScreenTooltip* tootip, m_tooltipList) {
        tootip->showToTopLeft();
    }
}

void MonitorGround::endEdit()
{
    foreach (Monitor *monitor, m_monitors) {
        monitor->setDraggable(false);
    }

    setEditing(false);

    foreach (FullScreenTooltip* tootip, m_tooltipList) {
        tootip->hide();
    }
}

bool MonitorGround::editable() const
{
    return m_editable;
}

bool MonitorGround::editing() const
{
    return m_editing;
}

void MonitorGround::setEditable(bool editable)
{
    m_editable = editable;
    if(!m_editable)
        endEdit();
    updateOpenedCount();
}

void MonitorGround::setEditing(bool editing)
{
    if (m_editing == editing)
        return;

    m_editing = editing;
    emit editingChanged(editing);
}

void MonitorGround::cancelEdit()
{
    foreach (Monitor *monitor, m_monitors) {
        monitor->resetResolution();
    }

    relayout();
}

void MonitorGround::relayout()
{
    QRect max_rect;

    QList<Monitor*> hide_monitors;

    foreach (Monitor *monitor, m_monitors) {
        monitor->setVisible(monitor->dbusInterface()->opened());

        if(monitor->isHidden()){
            hide_monitors << monitor;
            continue;
        }

        QRect rect = monitor->resolution();
        max_rect = rect.united(max_rect);
    }

    qreal rect_scale = qMin((double)width() / max_rect.width() * 0.9,
                            (double)height() / max_rect.height() * 0.6);

//    max_rect.setTop(max_rect.top() * rect_scale);
//    max_rect.setBottom(max_rect.bottom() * rect_scale);
//    max_rect.setLeft(max_rect.left() * rect_scale);
//    max_rect.setRight(max_rect.right() * rect_scale);
    max_rect.setWidth(max_rect.width() * rect_scale);
    max_rect.setHeight(max_rect.height() * rect_scale);
    max_rect.moveCenter(this->rect().center());

    foreach (Monitor *monitor, m_monitors) {
        QRect rect = monitor->resolution();

        if(monitor->isHidden()){
            continue;
        }

        foreach (Monitor *tmp, hide_monitors) {
            QRect tmp_rect = tmp->resolution();

            int tmp_int = rect.top() - tmp_rect.top();

            if(tmp_int > 0)
                rect.moveTop(rect.top() - qMin(tmp_int, tmp_rect.height()));

            tmp_int = rect.left() - tmp_rect.left();

            if(tmp_int > 0)
                rect.moveLeft(rect.left() - qMin(tmp_int, tmp_rect.width()));
        }

        rect.setTop(rect.top() * rect_scale);
        rect.setBottom(rect.bottom() * rect_scale);
        rect.setLeft(rect.left() * rect_scale);
        rect.setRight(rect.right() * rect_scale);
        rect.moveTopLeft(max_rect.topLeft() + rect.topLeft());

        monitor->setParentRect(max_rect);
        monitor->setGeometry(rect);
    }
}

void MonitorGround::onMonitorMouseMove()
{
    Monitor *monitor = qobject_cast<Monitor*>(sender());

    if(!monitor)
        return;

    foreach (Monitor *brother, m_monitors) {
        if(brother == monitor)
            continue;

        if(brother->geometry().contains(monitor->geometry().center())){
            brother->dragEnter(monitor);
        }else{
            brother->dragLeave();
        }
    }
}

void MonitorGround::onMonitorMouseRelease()
{
    Monitor *monitor = qobject_cast<Monitor*>(sender());

    if(!monitor)
        return;

    foreach (Monitor *brother, m_monitors) {
        if(brother == monitor)
            continue;

        if(brother->geometry().contains(monitor->geometry().center())){
            if(brother->drop(monitor)){
                brother->setDraggable(false);
                m_dbusDisplay->JoinMonitor(monitor->name(), brother->name());
                return;
            }
        }
    }

    QRect rect = monitor->resolution();

    rect.moveTopLeft(monitor->mapToRealPoint());

    monitor->setResolution(rect);

    QPoint min_pos = rect.topLeft();

    foreach (Monitor *monitor, m_monitors) {
        QRect rect = monitor->resolution();
        if(rect.x() < min_pos.x())
            min_pos.setX(rect.x());
        if(rect.y() < min_pos.y())
            min_pos.setY(rect.y());
    }
    foreach (Monitor *monitor, m_monitors) {
        QRect rect = monitor->resolution();
        rect.moveTopLeft(rect.topLeft() - min_pos);
        monitor->setResolution(rect);
    }

    relayout();
}

void MonitorGround::updateOpenedCount()
{
    int openedMonitorCount = 0;

    foreach (Monitor *monitor, m_monitors) {
        MonitorInterface *dbus = monitor->dbusInterface();
        if(dbus->opened()){
            ++openedMonitorCount;
            if(dbus->isComposited())
                ++openedMonitorCount;
        }
    }

    if(m_editable && openedMonitorCount > 1){
        if(openedMonitorCount > m_dbusDisplay->monitors().count()){
            m_split->show();
            m_edit->hide();
        }else{
            m_edit->show();
            m_split->hide();
        }
    }else{
        m_split->hide();
        m_edit->hide();
    }

    m_recognize->setVisible(openedMonitorCount > 1);
}
