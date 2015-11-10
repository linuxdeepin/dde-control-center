#ifndef ADDDSLPAGE_H
#define ADDDSLPAGE_H

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>

#include "dbus/dbusconnectionsession.h"

DUI_USE_NAMESPACE

class AddDslPage : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit AddDslPage(const QString &dbusPath, QWidget *parent = 0);

private:
    DBusConnectionSession *m_dbus;
};

#endif // ADDDSLPAGE_H
