#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <QMap>

#include <libdui/dboxwidget.h>

#include "dbus/dbusnetwork.h"

DUI_USE_NAMESPACE

class NetworkInfo : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit NetworkInfo(DBusNetwork *dbus, QWidget *parent = 0);

private:
    void init(DBusNetwork *dbus);

    QMap<QString, QString> m_mapConnectionTypeToName;
};

#endif // NETWORKINFO_H
