#ifndef CONNECTTOHIDDENAPPAGE_H
#define CONNECTTOHIDDENAPPAGE_H

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>

#include "dbus/dbusconnectionsession.h"
#include "networkglobal.h"

DUI_USE_NAMESPACE

class NetworkBaseEditLine;
class ConnectToHiddenApPage : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit ConnectToHiddenApPage(DBusConnectionSession *dbus, QWidget *parent = 0);

signals:
    void cancel();
    void confirm();

private:
    void init();

    DBusConnectionSession *m_dbus;
};

#endif // CONNECTTOHIDDENAPPAGE_H
