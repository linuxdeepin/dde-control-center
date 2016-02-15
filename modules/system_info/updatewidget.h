/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QMap>

#include <dlistwidget.h>
#include <dimagebutton.h>
#include <dseparatorhorizontal.h>
#include <dloadingindicator.h>

#include "updateprogress.h"
#include "applictionitemwidget.h"
#include "dbus/dbuslastoreupdater.h"
#include "dbus/dbusupdatejobmanager.h"
#include "dbus/dbusupdatejob.h"
#include "dbus/appupdateinfo.h"

DWIDGET_USE_NAMESPACE

class UpdateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpdateWidget(QWidget *parent = 0);

    static inline QString formatCap(qulonglong cap, const int size = 1024)
    {
        static QString type[] = {"B", "KB", "MB", "GB", "TB"};

        if (cap < qulonglong(size)) {
            return QString::number(cap) + type[0];
        }
        if (cap < qulonglong(size) * size) {
            return QString::number(double(cap) / size, 'f', 2) + type[1];
        }
        if (cap < qulonglong(size) * size * size) {
            return QString::number(double(cap) / size / size, 'f', 2) + type[2];
        }
        if (cap < qulonglong(size) * size * size * size) {
            return QString::number(double(cap) / size / size / size, 'f', 2) + type[3];
        }

        return QString::number(double(cap) / size / size / size / size, 'f', 2) + type[4];
    }

signals:
    void updatableNumsChanged(const int apps, const int packages) const;

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void systemUpgrade();
    void loadUpgradeJob(DBusUpdateJob *newJob);
    void toggleUpdateState();
    void disableAppsUpgrade();
    void loadCheckUpdateJob(DBusUpdateJob *updateJob);

private:
    enum UpgradeState {
        NotStart,
        SysCheckUpdate,
        SysUpGrading,
        SysFail,
    };

private slots:
    void loadAppList();
    void updateUpgradeProcess();
    void updateUpgradeState();
    void removeJob();
    void updateInfo(const int apps, const int packages);
    void checkUpdate();
    void refreshProgress(UpgradeState state);
    void restartUpgrade();
    void checkUpdateStateChanged();

private:
    QLabel *m_updateCountTips;
    QLabel *m_updateSizeTips;
    QLabel *m_updateStatTips;
    QWidget *m_tipsWidget;
    DLoadingIndicator *m_checkingIndicator;
    DImageButton *m_checkUpdateBtn;
    DImageButton *m_updateButton;
    UpdateProgress *m_updateProgress;
    DListWidget *m_appsList;
    DBusUpdateJob *m_dbusSystemUpgrade = nullptr;
    DBusUpdateJob *m_dbusCheckupdate = nullptr;
    DBusLastoreUpdater *m_dbusUpdateInter;
    DBusUpdateJobManager *m_dbusJobManagerInter;
    DSeparatorHorizontal *m_appSeparator;
    QList<QString> m_updatableAppsList;
    QList<QString> m_updatablePackagesList;

    UpgradeState m_upgradeStatus = NotStart;
};

#endif // UPDATEWIDGET_H
