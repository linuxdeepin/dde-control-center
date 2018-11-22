/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef TITLEEDIT_H
#define TITLEEDIT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc
{
namespace bluetooth
{
class TitleEdit : public QWidget
{
    Q_OBJECT
public:
    explicit TitleEdit(QWidget *parent = 0);

Q_SIGNALS:
    void requestSetBluetoothName(const QString &name);

public Q_SLOTS:
    void setName();
    void setEdit();
    void setTitle(const QString &title);

private:
    QLabel *m_name;
    QLineEdit *m_lineEdit;
    DImageButton *m_editWidget;

};
}
}


#endif // TITLEEDIT_H
