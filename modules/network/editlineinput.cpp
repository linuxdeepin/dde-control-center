#include <libdui/dlineedit.h>
#include <libdui/dpasswordedit.h>
#include <libdui/dconstants.h>

#include "editlineinput.h"

DUI_USE_NAMESPACE

EditLineInput::EditLineInput(const QString &section, const QString &key,
                             DBusConnectionSession *dbus, const QString &title,
                             EditLineInputType type,
                             QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    QWidget *line_edit;

    switch (type) {
    case EditLineInputType::Normal:
        line_edit = new DLineEdit;
        break;
    case EditLineInputType::Password:
        line_edit = new DPasswordEdit;
        break;
    case EditLineInputType::FileChooser:
        line_edit = new DLineEdit;
        break;
    default:
        break;
    }

    line_edit->setFixedSize(width() * 0.6, DUI::MENU_ITEM_HEIGHT);

    auto update_text = [this, line_edit] {
        line_edit->setProperty("text", cacheValue().toString());
    };

    connect(this, &NetworkBaseEditLine::widgetShown, this, update_text);
    connect(this, &NetworkBaseEditLine::cacheValueChanged, this, update_text);
    connect(line_edit, SIGNAL(textChanged(QString)), this, SLOT(setDBusKey(QString)));

    setRightWidget(line_edit);
}
