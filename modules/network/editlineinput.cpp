#include <libdui/dlineedit.h>
#include <libdui/dpasswordedit.h>
#include <libdui/dconstants.h>
#include <libdui/dfilechooseredit.h>
#include <libdui/dipv4lineedit.h>

#include "editlineinput.h"

DUI_USE_NAMESPACE

EditLineInput::EditLineInput(const QString &section, const QString &key,
                             DBusConnectionSession *dbus, const QString &title,
                             EditLineInputType type, QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    DLineEdit *line_edit = nullptr;

    switch (type) {
    case EditLineInputType::Normal:
        line_edit = new DLineEdit;
        break;
    case EditLineInputType::Password:
        line_edit = new DPasswordEdit;
        break;
    case EditLineInputType::FileChooser:{
        DFileChooserEdit *file_chooser = new DFileChooserEdit;
        line_edit = file_chooser;
        file_chooser->setDialogDisplayPosition(DFileChooserEdit::CurrentMonitorCenter);

        connect(file_chooser, &DFileChooserEdit::dialogOpened, this, [this] {
            window()->setProperty("autoHide", false);
        }, Qt::DirectConnection);
        connect(file_chooser, &DFileChooserEdit::dialogClosed, this, [this] {
            TIMER_SINGLESHOT(500, window()->setProperty("autoHide", true);, this);
        });
        break;
    }
    case EditLineInputType::Ipv4Input:{
        line_edit = new DIpv4LineEdit;
        break;
    }
    default:
        break;
    }

    if(line_edit) {
        auto update_text = [this, line_edit] {
            int current_seek = line_edit->cursorPosition();

            line_edit->setText(cacheValue().toString());
            line_edit->setCursorPosition(current_seek);
        };

        connect(this, &NetworkBaseEditLine::widgetShown, this, update_text);
        connect(this, &NetworkBaseEditLine::cacheValueChanged, this, update_text);
        connect(this, &NetworkBaseEditLine::readOnlyChanged, line_edit, &QLineEdit::setReadOnly);

        line_edit->setFixedSize(width() * 0.6, DUI::MENU_ITEM_HEIGHT);
        setRightWidget(line_edit);

        connect(this, &EditLineInput::showErrorAlert, line_edit, [line_edit]{
            line_edit->setProperty("alert", true);
        });
        connect(line_edit, &QLineEdit::textChanged, line_edit, [line_edit, this, title] {
            line_edit->setProperty("alert", false);
            setDBusKey(line_edit->text());
        });
        connect(line_edit, &DLineEdit::focusChanged, this, [this](bool focus) {
            if(!focus)
                checkKey();
        });

        if(!cacheValue().isNull())
            update_text();
    }
}
