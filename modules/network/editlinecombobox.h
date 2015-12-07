#ifndef EDITLINECOMBOBOX_H
#define EDITLINECOMBOBOX_H

#include <libdui/dcombobox.h>

#include "networkbaseeditline.h"

DUI_USE_NAMESPACE

class EditLineComboBox : public NetworkBaseEditLine
{
public:
    explicit EditLineComboBox(const QString &section, const QString &key,
                              DBusConnectionSession *dbus, const QString &title,
                              bool editable = false, QWidget *parent = 0);

    DComboBox *comboBox() const;

private:
    DComboBox *m_comboBox;
};

#endif // EDITLINECOMBOBOX_H
