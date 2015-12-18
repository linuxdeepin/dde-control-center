#include <QLineEdit>

#include <libdui/libdui_global.h>

#include "editlinecombobox.h"
#include "networkglobal.h"

EditLineComboBox::EditLineComboBox(const QString &section, const QString &key,
                                   DBusConnectionSession *dbus, const QString &title,
                                   bool editable, QWidget *parent) :
    NetworkBaseEditLine(section, key, dbus, title, parent)
{
    m_comboBox = new DComboBox;

    m_comboBox->setEditable(editable);

    connect(m_comboBox, static_cast<void (DComboBox::*)(int)>(&DComboBox::activated),
            this, [this](int index) {
        const QList<QJsonValue> &list = getAvailableValuesValue();

        if(index >= 0) {
            setDBusKey(list[index]);

            if(m_comboBox->isEditable()) {
                m_comboBox->lineEdit()->setText(cacheValue().toString());
            }
        }
    });

    if(editable) {
        m_comboBox->addItems(getAvailableValuesText());

        QLineEdit *line_edit = m_comboBox->lineEdit();

        auto update_text = [this, line_edit] {
            int current_seek = line_edit->cursorPosition();
            line_edit->setText(cacheValue().toString());
            line_edit->setCursorPosition(current_seek);
        };

        connect(this, &NetworkBaseEditLine::widgetShown, this, update_text);
        connect(this, &NetworkBaseEditLine::cacheValueChanged, this, update_text);
        connect(line_edit, SIGNAL(textEdited(QString)), this, SLOT(setDBusKey(QString)));
        connect(this, &EditLineComboBox::readOnlyChanged, line_edit, &QLineEdit::setReadOnly);
        connect(this, &EditLineComboBox::showErrorAlert, this, [this]{
            m_comboBox->setAlert(true);
        });
        connect(line_edit, &QLineEdit::textChanged, this, [this]{
            m_comboBox->setAlert(false);
        });

        if(cacheValue().isNull())
            update_text();
    } else {
        auto updateComboData = [this]{
            const QStringList &list = getAvailableValuesText();

            m_comboBox->clear();
            m_comboBox->addItems(list);

            int index = getAvailableValuesIndex();

            if(index >= 0)
                m_comboBox->setCurrentIndex(index);
        };


        connect(this, &NetworkBaseEditLine::widgetShown, this, updateComboData);
        connect(this, &NetworkBaseEditLine::cacheValueChanged, this, updateComboData);
    }

    m_comboBox->setFixedSize(width() * 0.6, DUI::MENU_ITEM_HEIGHT);

    setRightWidget(m_comboBox);
}

DComboBox *EditLineComboBox::comboBox() const
{
    return m_comboBox;
}

