/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MONITORGROUND_H
#define MONITORGROUND_H

#include <QFrame>

class Monitor;
class DisplayInterface;
class DisplayModeItem;
class FullScreenTooltip;
class MonitorGround : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool editable READ editable WRITE setEditable)
    Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)

public:
    explicit MonitorGround(DisplayInterface *display, QWidget *parent = 0);
    ~MonitorGround();

    void addMonitor(Monitor *monitor);
    void removeMonitor(Monitor *monitor);
    void clear();
    bool editable() const;
    bool editing() const;

public slots:
    void setEditable(bool editable);
    void setEditing(bool editing);
    void cancelEdit();

signals:
    void editingChanged(bool editing);
    void applyEdit();

private slots:
    void beginEdit();
    void endEdit();
    void relayout();
    void onMonitorMouseMove();
    void onMonitorMouseRelease();
    void updateOpenedCount();

private:
    bool m_editing = false;
    bool m_editable;
    QList<Monitor *> m_monitors;
    DisplayInterface *m_dbusDisplay;
    DisplayModeItem *m_recognize;
    DisplayModeItem *m_edit;
    DisplayModeItem *m_split;
    QList<FullScreenTooltip *> m_tooltipList;
};

#endif // MONITORGROUND_H
