#include <libdui/dlineedit.h>
#include <libdui/dpasswordedit.h>
#include <libdui/dconstants.h>
#include <libdui/dfilechooseredit.h>
#include <libdui/dipv4lineedit.h>
#include <libdui/dspinbox.h>

#include "editlineinput.h"

DUI_USE_NAMESPACE

EditLineInput::EditLineInput(const QString &section, const QString &key,
                             DBusConnectionSession *dbus, const QString &title,
                             EditLineInputType type, int minValue, int maxValue,
                             QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    QLineEdit *line_edit = nullptr;

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
    case EditLineInputType::SpinBox:{
        DSpinBox *box = new DSpinBox;
        line_edit = box->lineEdit();
        box->setMinimum(minValue);
        box->setMaximum(maxValue);
        setRightWidget(box);
        break;
    }
    default:
        break;
    }

    if(line_edit) {
        line_edit->setFixedSize(width() * 0.6, DUI::MENU_ITEM_HEIGHT);

        auto update_text = [this, line_edit] {
            int current_seek = line_edit->cursorPosition();
            line_edit->setText(cacheValue().toString());
            line_edit->setCursorPosition(current_seek);
        };

        connect(this, &NetworkBaseEditLine::widgetShown, this, update_text);
        connect(this, &NetworkBaseEditLine::cacheValueChanged, this, update_text);
        connect(line_edit, SIGNAL(textChanged(QString)), this, SLOT(setDBusKey(QString)));
        connect(this, &EditLineInput::showErrorAlert, line_edit, [line_edit]{
            line_edit->setProperty("alert", true);
        });
        connect(line_edit, &QLineEdit::textChanged, line_edit, [line_edit]{
            line_edit->setProperty("alert", false);
        });

        if(type != EditLineInputType::SpinBox)
            setRightWidget(line_edit);
    }
}
