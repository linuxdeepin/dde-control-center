#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include "settingsitem.h"

#include <dswitchbutton.h>

#include <QLabel>

namespace dcc {

class SwitchWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit SwitchWidget(QFrame *parent = 0);

    void setChecked(const bool checked = true);
    void setTitle(const QString& title);

signals:
    void checkedChanegd(const bool checked) const;

private:
    QLabel *m_title;
    Dtk::Widget::DSwitchButton *m_switchBtn;
};

}

#endif // SWITCHWIDGET_H
