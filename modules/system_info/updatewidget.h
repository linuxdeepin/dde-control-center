#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QMap>

#include <libdui/dcircleprogress.h>
#include <libdui/dlistwidget.h>

#include "applictionitemwidget.h"
#include "dbus/dbuslastoremanager.h"
#include "dbus/dbusupdatejobmanager.h"

DUI_USE_NAMESPACE

class UpdateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpdateWidget(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void loadAppList();
    void updateTipsInfo();

private:
    QLabel *m_updateCountTips;
    QLabel *m_updateSizeTips;
    DCircleProgress *m_updateProgress;
    DListWidget *m_appsList;
    DBusLastoreManager *m_dbusUpdateInter;
    DBusUpdateJobManager *m_dbusJobManagerInter;
//    QMap<QListWidgetItem *, ApplictionItemWidget *> *m_appItems;
};

#endif // UPDATEWIDGET_H
