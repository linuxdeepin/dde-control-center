#ifndef EDITLINESPINBOX_H
#define EDITLINESPINBOX_H

#include "networkbaseeditline.h"

class EditLineSpinBox : public NetworkBaseEditLine
{
    Q_OBJECT
public:
    explicit EditLineSpinBox(const QString &section, const QString &key,
                             DBusConnectionSession *dbus, const QString &title,
                             int minValue = 0, int maxValue = 0, QWidget *parent = 0);
};

#endif // EDITLINESPINBOX_H
