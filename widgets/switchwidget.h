#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include "settingsitem.h"

#include <dswitchbutton.h>

namespace dcc {
namespace widgets {

class SwitchWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit SwitchWidget(QFrame *parent = 0);
    explicit SwitchWidget(const QString &title, QFrame *parent = 0);
    explicit SwitchWidget(QWidget *widget, QFrame *parent = 0);

    void setChecked(const bool checked = true);
    void setTitle(const QString& title);
    bool checked() const;

signals:
    void checkedChanged(const bool checked) const;

private:
    QWidget *m_leftWidget;
    Dtk::Widget::DSwitchButton *m_switchBtn;
};

}
}

#endif // SWITCHWIDGET_H
