#include <libdui/dspinbox.h>
#include <libdui/dconstants.h>

#include "editlinespinbox.h"
#include "networkglobal.h"

DUI_USE_NAMESPACE

EditLineSpinBox::EditLineSpinBox(const QString &section, const QString &key,
                                 DBusConnectionSession *dbus, const QString &title,
                                 int minValue, int maxValue, QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    DSpinBox *box = new DSpinBox;

    box->setMinimum(minValue);
    box->setMaximum(maxValue);

    box->setFixedSize(width() * 0.6, DUI::MENU_ITEM_HEIGHT);

    connect(this, &EditLineSpinBox::showErrorAlert, box, [box]{
        box->setAlert(true);
    });

    auto update_text = [this, box] {
        SIGNAL_BLOCKE(box)
        box->setValue(cacheValue().toInt());
    };

    connect(this, &NetworkBaseEditLine::widgetShown, this, update_text);
    connect(this, &NetworkBaseEditLine::cacheValueChanged, this, update_text);
    connect(box, static_cast<void (DSpinBox::*)(int)>(&DSpinBox::valueChanged), this, [this, box](int value) {
        box->setAlert(false);
        setDBusKey(value);
    });

    if(!cacheValue().isNull())
        update_text();

    setRightWidget(box);
}

