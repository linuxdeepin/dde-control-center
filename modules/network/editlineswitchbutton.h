#ifndef EDITLINESWITCHBUTTON_H
#define EDITLINESWITCHBUTTON_H

#include "networkbaseeditline.h"

class EditLineSwitchButton : public NetworkBaseEditLine
{
public:
    explicit EditLineSwitchButton(const QString &section, const QString &key,
                                 DBusConnectionSession *dbus, const QString &title,
                                 QWidget *parent = 0);
};

#endif // EDITLINESWITCHBUTTON_H
