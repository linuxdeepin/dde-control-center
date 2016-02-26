/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef RUNNABLETASK_H
#define RUNNABLETASK_H

#include <QObject>
#include <QRunnable>
#include "defaultapps.h"
#include "helper.h"

class DefaultApps;

class RunnableTask : public QThread
{
    Q_OBJECT

public:
    RunnableTask(QMap<DefaultApps::DefaultAppsCategory, DArrowLineExpand *> taskMap, QMap<DefaultApps::DefaultAppsCategory, DOptionList *> *appBtnMap);
    void run();
    DArrowLineExpand *createDefaultAppsExpand(const DefaultApps::DefaultAppsCategory &category, DArrowLineExpand *defaultApps);

signals:
    void optionListReady(DOptionList *);
    void appListReady(QJsonArray json, int acategory, DArrowLineExpand *arrowLineApps);

private:
    const QString getTypeByCategory(const DefaultApps::DefaultAppsCategory &category);
    bool isMediaApps(const DefaultApps::DefaultAppsCategory &category) const;
    const QStringList getTypeListByCategory(const DefaultApps::DefaultAppsCategory &category);
    void updateCheckedItem(const DefaultApps::DefaultAppsCategory &category);

    DefaultApps::DefaultAppsCategory m_category;
    DArrowLineExpand *m_arrowLineExpand;
    DefaultApps *m_defaultApps;
    DBusDefaultApps m_dbusDefaultApps;
    DBusDefaultMedia m_dbusDefaultMedia;
    QMap<DefaultApps::DefaultAppsCategory, DOptionList *> *m_appsBtnMap;
    QMap<DefaultApps::DefaultAppsCategory, DArrowLineExpand *> m_taskMap;
    DExpandGroup *m_appGrp;
    DOptionList *m_list;








};

#endif // RUNNABLETASK_H
