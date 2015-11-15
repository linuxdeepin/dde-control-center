#ifndef EDITLINEINPUT_H
#define EDITLINEINPUT_H

#include "networkbaseeditline.h"
#include "networkglobal.h"

class EditLineInput : public NetworkBaseEditLine
{
public:
    enum EditLineInputType {
        Normal,
        Password,
        FileChooser
    };

    explicit EditLineInput(const QString &section, const QString &key,
                           DBusConnectionSession *dbus, const QString &title,
                           EditLineInputType type = Normal,
                           QWidget *parent = 0);
};

#endif // EDITLINEINPUT_H
