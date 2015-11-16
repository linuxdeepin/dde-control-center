#ifndef SYSTEMPROXYWIDGET_H
#define SYSTEMPROXYWIDGET_H

#include <libdui/dboxwidget.h>

#include "dbus/dbusnetwork.h"

DUI_USE_NAMESPACE

class SystemProxyWidget : public DVBoxWidget
{
public:
    explicit SystemProxyWidget(DBusNetwork *dbus, QWidget *parent = 0);

private:
    int m_currentIndex = 0;
};

#endif // SYSTEMPROXYWIDGET_H
