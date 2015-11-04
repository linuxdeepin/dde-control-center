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
#include "dbus/dbuslastoremanager.h"
#include "dbus/dbusupdatejobmanager.h"

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
    void toggleUpdateState();

private slots:
    void loadAppList();

private:
    QLabel *m_updateCountTips;
    QLabel *m_updateSizeTips;
    DImageButton *m_updateButton;
    DCircleProgress *m_updateProgress;
    DListWidget *m_appsList;
    DBusLastoreManager *m_dbusUpdateInter;
    DBusUpdateJobManager *m_dbusJobManagerInter;
};

#endif // UPDATEWIDGET_H
