/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef GRUBWIDGET_H
#define GRUBWIDGET_H

#include <QFrame>
#include <QVBoxLayout>

#include <libdui_global.h>
#include <dheaderline.h>
#include <dexpandgroup.h>
#include <darrowlineexpand.h>

#include "shortcutdbus.h"
#include "shortcutwidget.h"
#include "scrollframe.h"

DWIDGET_USE_NAMESPACE

class AddRmDoneLine;
class ModuleHeader;
class SearchList;
class SelectDialog;
class MainWidget : public ScrollFrame
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

signals:
    void addCustomShortcut();
    void addCustomShortcutFinished();
    void setEnableEditShortcut(bool enable);

private:
    QBoxLayout *m_layout;
    QVBoxLayout *m_childLayout;
    ModuleHeader *m_header;
    ShortcutDbus *m_dbus;
    SearchList *m_searchList;
    SearchList *m_systemList;
    SearchList *m_windowList;
    SearchList *m_workspaceList;
    SearchList *m_customList;
    DExpandGroup *m_expandGroup;
    AddRmDoneLine *m_customLine;
    QWidget *m_addShortcutDialog;
    QList<QWidget *> m_expandGroupChildList;

    QPointer<SelectDialog> m_conflictDialog;

    void init();

    SearchList *addSearchList(const ShortcutInfoList &tmplist);
    Q_SLOT void shortcutListChanged(SearchList *listw, const ShortcutInfoList &list, int offseIndex);
    Q_SLOT void editShortcut(ShortcutWidget *w, SearchList *listw, const QString &flag, QString shortcut);
    Q_SLOT void removeShortcut();
    QWidget *getAddShortcutWidget();
    AddRmDoneLine *getCustomLstHeadBar();
    DArrowLineExpand *addExpand(const QString &title, QWidget *widget);
};

#endif // GRUBWIDGET_H
