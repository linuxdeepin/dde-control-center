#ifndef CONNECTTOHIDDENAPPAGE_H
#define CONNECTTOHIDDENAPPAGE_H

#include "dbus/dbusconnectionsession.h"
#include "networkglobal.h"
#include "listwidgetcontainer.h"

class NetworkBaseEditLine;
class ConnectToHiddenApPage : public ListWidgetContainer
{
    Q_OBJECT
public:
    explicit ConnectToHiddenApPage(DBusConnectionSession *dbus, QWidget *parent = 0);
    ~ConnectToHiddenApPage();

signals:
    void cancel();
    void confirm();

private:
    void init();

    DBusConnectionSession *m_dbus;
};

#endif // CONNECTTOHIDDENAPPAGE_H
