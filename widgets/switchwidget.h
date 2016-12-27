#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include "settingsitem.h"
#include "labels/normallabel.h"

#include <dswitchbutton.h>

#include <QLabel>

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

signals:
    void checkedChanegd(const bool checked) const;

private:
    NormalLabel *m_title;
    Dtk::Widget::DSwitchButton *m_switchBtn;
};

}
}

#endif // SWITCHWIDGET_H
