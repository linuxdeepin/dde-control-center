#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QMap>

#include <libdui/dcircleprogress.h>
#include <libdui/dlistwidget.h>
#include <libdui/dimagebutton.h>

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

    static QString formatCap(qulonglong cap);

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

private:
    QLabel *m_updateCountTips;
    QLabel *m_updateSizeTips;
    DImageButton *m_updateButton;
    DCircleProgress *m_updateProgress;
    DListWidget *m_appsList;
    DBusUpdateJob *m_dbusSystemUpgrade = nullptr;
    DBusLastoreUpdater *m_dbusUpdateInter;
    DBusUpdateJobManager *m_dbusJobManagerInter;
};

#endif // UPDATEWIDGET_H
