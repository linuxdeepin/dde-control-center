/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include "widgets/settingsitem.h"

#include <QHBoxLayout>

#include <dswitchbutton.h>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

class SwitchWidget : public SettingsItem
{
    Q_OBJECT

public:
//    explicit SwitchWidget(QWidget *parent = nullptr);
    explicit SwitchWidget(const QString &title, QWidget *parent = nullptr);
    explicit SwitchWidget(QWidget *parent = nullptr, QWidget *widget = nullptr);

    void setChecked(const bool checked = true);
    QString title() const;
    void setTitle(const QString &title);
    bool checked() const;

    QWidget *leftWidget() const { return m_leftWidget; }
    QHBoxLayout* getMainLayout() { return m_mainLayout; }

public:
    inline DTK_WIDGET_NAMESPACE::DSwitchButton *switchButton() const { return m_switchBtn; }

Q_SIGNALS:
    void checkedChanged(const bool checked) const;
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    QWidget *m_leftWidget;
    Dtk::Widget::DSwitchButton *m_switchBtn;
    QHBoxLayout *m_mainLayout;
};

}
}

#endif // SWITCHWIDGET_H
