#include <libdui/dswitchbutton.h>

#include "editlineswitchbutton.h"

DUI_USE_NAMESPACE

EditLineSwitchButton::EditLineSwitchButton(const QString &section, const QString &key,
                                           DBusConnectionSession *dbus, const QString &title,
                                           QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    DSwitchButton *switch_button = new DSwitchButton;

    auto updateSwitchButtonCheckeState = [switch_button, this] {
        switch_button->setChecked(cacheValue().toBool());
    };

    connect(this, &NetworkBaseEditLine::widgetShown, this, updateSwitchButtonCheckeState);
    connect(this, &NetworkBaseEditLine::cacheValueChanged, this, updateSwitchButtonCheckeState);
    connect(switch_button, &DSwitchButton::checkedChanged, this, [this](bool checked) {
        setDBusKey(checked);
    });

    setRightWidget(switch_button);

    if(cacheValue().isNull())
        updateSwitchButtonCheckeState();
}
