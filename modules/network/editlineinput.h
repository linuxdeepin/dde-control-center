#ifndef EDITLINEINPUT_H
#define EDITLINEINPUT_H

#include "networkbaseeditline.h"
#include "networkglobal.h"

class EditLineInput : public NetworkBaseEditLine
{
public:
    explicit EditLineInput(const QString &section, const QString &key,
                           DBusConnectionSession *dbus, const QString &title,
                           BaseLineEditType::BaseLineEditType type = BaseLineEditType::Normal,
                           QWidget *parent = 0);
};

#endif // EDITLINEINPUT_H
