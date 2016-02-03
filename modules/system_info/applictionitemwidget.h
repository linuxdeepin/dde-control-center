/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPLICTIONITEMWIDGET_H
#define APPLICTIONITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QDBusObjectPath>

#include <libdui/dimagebutton.h>
#include <libdui/dcircleprogress.h>

#include "separatorwidget.h"
#include "dbus/appupdateinfo.h"
#include "dbus/dbusupdatejobmanager.h"
#include "dbus/dbusupdatejob.h"

DUI_USE_NAMESPACE

class ApplictionItemWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ selected NOTIFY selectStateChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoverStateChanged)

public:
    enum Status {
        Ready,
        Updating,
        SysUpgrading,
        Fail,
        Success,
    };

signals:
    void progressChanged(int percent) const;
    void jobStarted() const;
    void jobStatChanged() const;
    void selectStateChanged() const;
    void hoverStateChanged() const;
    void jobFinished() const;

public:
    explicit ApplictionItemWidget(QWidget *parent = 0);

    inline bool hovered() const {return m_hovered;}
    inline bool selected() const {return m_selected;}
    inline void setAppName(const QString &name) {m_appName->setText(name);}
    inline void setAppVer(const QString &ver) {m_appVersion->setText(ver);}
    inline void setAppIcon(const QPixmap &icon) {m_appIcon->setPixmap(icon);}
    void setAppUpdateInfo(const AppUpdateInfo &info);
    void disableUpdate();

public slots:
    void connectToJob(DBusUpdateJob *dbusJob);
    inline void hideSeparator() {m_separator->hide();}

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    void toggleUpdateJob();
    void startJob();
    void updateJobProgress();
    void updateJobStatus();
    void updateJobStat(Status stat);
    void progressClicked();
    void restartJob();
    const QString getIconPath(const AppUpdateInfo &info) const;

private:
    QLabel *m_appIcon;
    QLabel *m_appName;
    QLabel *m_appVersion;
    DCircleProgress *m_progress;
    QPushButton *m_updateBtn;
    DBusUpdateJobManager *m_dbusJobManagerInter;
    DBusUpdateJob *m_dbusJobInter = nullptr;
    HSeparatorWidget *m_separator;

    Status m_stat = Ready;

    AppUpdateInfo m_updateInfo;

    bool m_disableUpdate = false;
    bool m_selected = false;
    bool m_jobRunning = false;
    bool m_hovered = false;
};

#endif // APPLICTIONITEMWIDGET_H
