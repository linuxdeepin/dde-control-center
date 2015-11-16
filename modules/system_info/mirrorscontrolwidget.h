#ifndef MIRRORSCONTROLWIDGET_H
#define MIRRORSCONTROLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMap>

#include <libdui/dswitchbutton.h>
#include <libdui/dtextbutton.h>
#include <libdui/dlistwidget.h>

#include "mirroritemwidget.h"
#include "dbus/dbuslastoreupdater.h"

DUI_USE_NAMESPACE

class MirrorsControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MirrorsControlWidget(QWidget *parent = 0);
    ~MirrorsControlWidget();

signals:
    void configAccept() const;

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void loadMirrorList();

private:
    DSwitchButton *m_updateSwitchBtn;
    DTextButton *m_changeMirrorBtn;
    DTextButton *m_applyBtn;
    DListWidget *m_mirrorsList;

    DBusLastoreUpdater *m_dbusLastoreInter;
};

#endif // MIRRORSCONTROLWIDGET_H
