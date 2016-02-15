/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MIRRORSCONTROLWIDGET_H
#define MIRRORSCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMap>

#include <dswitchbutton.h>
#include <dtextbutton.h>
#include <dlistwidget.h>

#include "mirroritemwidget.h"
#include "dbus/dbuslastoreupdater.h"

DWIDGET_USE_NAMESPACE

class MirrorsControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MirrorsControlWidget(QWidget *parent = 0);
    ~MirrorsControlWidget();

signals:
    void configAccept() const;

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void loadMirrorList();

private:
    DSwitchButton *m_updateSwitchBtn;
    DTextButton *m_changeMirrorBtn;
    DTextButton *m_applyBtn;
    DListWidget *m_mirrorsList;

    DBusLastoreUpdater *m_dbusLastoreInter;
};

#endif // MIRRORSCONTROLWIDGET_H
