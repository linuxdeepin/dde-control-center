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

#ifndef CHECKITEM_H
#define CHECKITEM_H

#include "widgets/settingsitem.h"
#include <dimagebutton.h>
#include <QPushButton>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard{
class CheckItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit CheckItem(QFrame *parent = 0);
    void setTitle(const QString& title);
    QString title() const;

    void setChecked(bool checked);
    void setMultipleMode(bool multiple);
    bool checked() const;

Q_SIGNALS:
    void checkedChanged(const QString &title);
    void destroySelf(CheckItem* item);

public Q_SLOTS:
    void onEditMode(bool value);
    void onClick();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool m_checked;
    bool m_bEdit;
    QLabel* m_title;
    DImageButton* m_checkBtn;
    int m_circle;
    bool m_multiple;
};
}
}
#endif // CHECKITEM_H
