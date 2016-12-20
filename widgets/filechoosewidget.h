#ifndef FILECHOOSEWIDGET_H
#define FILECHOOSEWIDGET_H

#include "settingsitem.h"

#include <dfilechooseredit.h>

class QLabel;

namespace dcc {

namespace widgets {

class FileChooseWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit FileChooseWidget(QWidget *parent = 0);

    Dtk::Widget::DFileChooserEdit *edit() const { return m_edit; }
    void setTitle(const QString &title);

private:
    QLabel *m_title;
    Dtk::Widget::DFileChooserEdit *m_edit;
};

}

}

#endif // FILECHOOSEWIDGET_H
