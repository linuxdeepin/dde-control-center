#ifndef COMBOBOXWIDGET_H
#define COMBOBOXWIDGET_H

#include "settingsitem.h"

class QLabel;
class QComboBox;

namespace dcc {

namespace widgets {

class ComboBoxWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit ComboBoxWidget(QWidget *parent = 0);

    QComboBox *comboBox() const { return m_comboBox; }
    void setTitle(const QString &title);

private:
    QLabel *m_title;
    QComboBox *m_comboBox;
};

}

}

#endif // COMBOBOXWIDGET_H
