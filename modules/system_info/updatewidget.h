#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QMap>

#include <libdui/dcircleprogress.h>
#include <libdui/dlistwidget.h>
#include <libdui/dimagebutton.h>
#include <libdui/dseparatorhorizontal.h>

#include "applictionitemwidget.h"
#include "dbus/dbuslastoreupdater.h"
#include "dbus/dbusupdatejobmanager.h"
#include "dbus/dbusupdatejob.h"
#include "dbus/appupdateinfo.h"

DUI_USE_NAMESPACE

class UpdateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpdateWidget(QWidget *parent = 0);

    static inline QString formatCap(qulonglong cap, const int size = 1024)
    {
        static QString type[] = {"B", "KB", "MB", "GB", "TB"};

        if (cap < qulonglong(size))
            return QString::number(cap) + type[0];
        if (cap < qulonglong(size) * size)
            return QString::number(double(cap) / size, 'f', 2) + type[1];
        if (cap < qulonglong(size) * size * size)
            return QString::number(double(cap) / size / size, 'f', 2) + type[2];
        if (cap < qulonglong(size) * size * size * size)
            return QString::number(double(cap) / size / size / size, 'f', 2) + type[3];

        return QString::number(double(cap) / size / size / size / size, 'f', 2) + type[4];
    }

signals:
    void updatableNumsChanged(const int nums) const;

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void systemUpgrade();
    void loadUpgradeData(DBusUpdateJob *newJob);
    void toggleUpdateState();

private slots:
    void loadAppList();
    void updateUpgradeProcess();
    void updateUpgradeState();
    void removeJob();
    void updateInfo(const int updatableAppsNum);

private:
    QLabel *m_updateCountTips;
    QLabel *m_updateSizeTips;
    DImageButton *m_updateButton;
    DCircleProgress *m_updateProgress;
    DListWidget *m_appsList;
    DBusUpdateJob *m_dbusSystemUpgrade = nullptr;
    DBusLastoreUpdater *m_dbusUpdateInter;
    DBusUpdateJobManager *m_dbusJobManagerInter;
    DSeparatorHorizontal *m_appSeparator;
};

#endif // UPDATEWIDGET_H
