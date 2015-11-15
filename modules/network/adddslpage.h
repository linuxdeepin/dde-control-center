#ifndef ADDDSLPAGE_H
#define ADDDSLPAGE_H

#include "listwidgetcontainer.h"
#include "dbus/dbusconnectionsession.h"

class AddDslPage : public ListWidgetContainer
{
public:
    explicit AddDslPage(const QString &dbusPath, QWidget *parent = 0);

private:
    DBusConnectionSession *m_dbus;
};

#endif // ADDDSLPAGE_H
