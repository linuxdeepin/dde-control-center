#ifndef ADDVPNPAGE_H
#define ADDVPNPAGE_H

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>

#include "dbus/dbusconnectionsession.h"

DUI_USE_NAMESPACE

class AddVpnPage : public DVBoxWidget
{
    Q_OBJECT

public:
    explicit AddVpnPage(const QString &dbusPath, QWidget *parent = 0);

private:
    void init();

    DBusConnectionSession *m_dbus;
    QMap<QString,QString> m_mapVpnNameToType;
};

#endif // ADDVPNPAGE_H
