#ifndef FILECHOOSEWIDGET_H
#define FILECHOOSEWIDGET_H

#include "settingsitem.h"

#include <dimagebutton.h>

class QLabel;
class QLineEdit;

namespace dcc {

namespace widgets {

class FileChooseWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit FileChooseWidget(QWidget *parent = 0);

    QLineEdit *edit() const { return m_edit; }
    void setTitle(const QString &title);

signals:
    void requestFrameKeepAutoHide(const bool autoHide) const;

private slots:
    void chooseFile();

private:
    QLabel *m_title;
    QLineEdit *m_edit;
    Dtk::Widget::DImageButton *m_btn;
};

}

}

#endif // FILECHOOSEWIDGET_H
