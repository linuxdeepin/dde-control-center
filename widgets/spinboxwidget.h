#ifndef SPINBOXWIDGET_H
#define SPINBOXWIDGET_H

#include "settingsitem.h"

class QLabel;

namespace Dtk {

namespace Widget {

    class DSpinBox;

}

}

namespace dcc {

namespace widgets {

class SpinBoxWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit SpinBoxWidget(QWidget *parent = 0);

    Dtk::Widget::DSpinBox *spinBox() const { return m_spinBox; }

    void setTitle(const QString &title);

private:
    QLabel *m_title;
    Dtk::Widget::DSpinBox *m_spinBox;
};

}

}

#endif // SPINBOXWIDGET_H
