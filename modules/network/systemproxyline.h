#ifndef SYSTEMPROXYLINE_H
#define SYSTEMPROXYLINE_H

#include <libdui/dheaderline.h>

#include "networkglobal.h"
#include "dbus/dbusnetwork.h"

DUI_USE_NAMESPACE

class SystemProxyLine : public DHeaderLine
{
public:
    explicit SystemProxyLine(const QString &title, const QString &proxyType,
                             DBusNetwork *dbus, QWidget *parent = 0);
};

#endif // SYSTEMPROXYLINE_H
