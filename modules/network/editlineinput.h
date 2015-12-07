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
        FileChooser,
        Ipv4Input,
        SpinBox
    };

    explicit EditLineInput(const QString &section, const QString &key,
                           DBusConnectionSession *dbus, const QString &title,
                           EditLineInputType type = Normal, int minValue = 0, int maxValue = 0,
                           QWidget *parent = 0);
};

#endif // EDITLINEINPUT_H
