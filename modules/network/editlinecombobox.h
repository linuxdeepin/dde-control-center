#ifndef EDITLINECOMBOBOX_H
#define EDITLINECOMBOBOX_H

#include "networkbaseeditline.h"

class EditLineComboBox : public NetworkBaseEditLine
{
public:
    explicit EditLineComboBox(const QString &section, const QString &key,
                              DBusConnectionSession *dbus, const QString &title,
                              QWidget *parent = 0);
};

#endif // EDITLINECOMBOBOX_H
