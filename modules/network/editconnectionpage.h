#ifndef EDITCONNECTIONPAGE_H
#define EDITCONNECTIONPAGE_H

#include <QWidget>

#include "listwidgetcontainer.h"
#include "networkbaseeditline.h"

DUI_USE_NAMESPACE

class DBusConnectionSession;
class EditConnectionPage : public ListWidgetContainer
{
    Q_OBJECT
public:
    explicit EditConnectionPage(const QString &dbusPath, QWidget *parent = 0);
    ~EditConnectionPage();

private:
    NetworkBaseEditLine *getLineByMap(const QVariantMap &map);

    DBusConnectionSession *m_dbus;
};

#endif // EDITCONNECTIONPAGE_H
