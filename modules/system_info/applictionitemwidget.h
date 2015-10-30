#ifndef APPLICTIONITEMWIDGET_H
#define APPLICTIONITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QDBusObjectPath>

#include <libdui/dimagebutton.h>
#include <libdui/dcircleprogress.h>

#include "dbus/appupdateinfo.h"
#include "dbus/dbusupdatejobmanager.h"
#include "dbus/dbusupdatejob.h"

DUI_USE_NAMESPACE

class ApplictionItemWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ selected NOTIFY selectStateChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoverStateChanged)

signals:
    void progressChanged(int percent) const;
    void jobStarted() const;
    void jobStatChanged() const;
    void selectStateChanged() const;
    void hoverStateChanged() const;

public:
    explicit ApplictionItemWidget(QWidget *parent = 0);

    inline bool hovered() const {return m_hovered;}
    inline bool selected() const {return m_selected;}
    void setAppUpdateInfo(const AppUpdateInfo &info);

public slots:
    void connectToJob(const QDBusObjectPath &jobPath);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private:
    void toggleUpdateJob();
    void startJob();
    void updateJobProgress();
    void updateJobStatus();

private:
    QLabel *m_appIcon;
    QLabel *m_appName;
    QLabel *m_appVersion;
    DCircleProgress *m_progress;
    QPushButton *m_updateBtn;
    DBusUpdateJobManager *m_dbusJobManagerInter;
    DBusUpdateJob *m_dbusJobInter = nullptr;

    AppUpdateInfo m_updateInfo;

    bool m_selected = false;
    bool m_jobRunning = false;
    bool m_hovered = false;
};

#endif // APPLICTIONITEMWIDGET_H
