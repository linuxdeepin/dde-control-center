#include <libdui/dcombobox.h>
#include <libdui/libdui_global.h>

#include "editlinecombobox.h"

DUI_USE_NAMESPACE

EditLineComboBox::EditLineComboBox(const QString &section, const QString &key,
                                   DBusConnectionSession *dbus, const QString &title,
                                   bool editable, QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    DComboBox *combo_box = new DComboBox;

    combo_box->setEditable(editable);

    auto updateComboData = [this, combo_box]{
        const QStringList &list = getAvailableValuesText();

        combo_box->clear();
        combo_box->addItems(list);

        int index = getAvailableValuesIndex();

        if(index >= 0)
            combo_box->setCurrentIndex(index);
    };

    combo_box->setFixedSize(width() * 0.6, DUI::MENU_ITEM_HEIGHT);

    connect(this, &NetworkBaseEditLine::widgetShown, this, updateComboData);
    connect(this, &NetworkBaseEditLine::cacheValueChanged, this, updateComboData);
    connect(combo_box, static_cast<void (DComboBox::*)(int)>(&DComboBox::activated),
            this, [this, combo_box](int index) {
        const QList<QJsonValue> &list = getAvailableValuesValue();

        if(index >= 0) {
            setDBusKey(list[index]);
        }
    });

    setRightWidget(combo_box);
}

