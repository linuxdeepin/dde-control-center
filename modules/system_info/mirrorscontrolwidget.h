#ifndef MIRRORSCONTROLWIDGET_H
#define MIRRORSCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMap>

#include <libdui/dswitchbutton.h>
#include <libdui/dtextbutton.h>
#include <libdui/dlistwidget.h>

#include "mirroritemwidget.h"
#include "dbus/dbuslastoremanager.h"

DUI_USE_NAMESPACE

class MirrorsControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MirrorsControlWidget(QWidget *parent = 0);
    ~MirrorsControlWidget();

    inline const QStringList updateableAppList() const {return m_dbusLastoreInter->updatableApps1();}

signals:
    void configAccept() const;

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void loadMirrorList();

private:
    DSwitchButton *m_updateSwitchBtn;
    DTextButton *m_changeMirrorBtn;
    DTextButton *m_applyBtn;
    DListWidget *m_mirrorsList;

    DBusLastoreManager *m_dbusLastoreInter;
};

#endif // MIRRORSCONTROLWIDGET_H
