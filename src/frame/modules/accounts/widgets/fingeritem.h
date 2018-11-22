/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             Hualet <mr.asianwang@gmail.com>
 *             kirigaya <kirigaya@mkacg.com>
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

#ifndef FINGERITEM_H
#define FINGERITEM_H

#include "widgets/settingsitem.h"
#include <QWidget>
#include <QHBoxLayout>
#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace accounts {
class FingerItem : public widgets::SettingsItem
{
    Q_OBJECT
public:
    explicit FingerItem(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setEditMode(const bool editmode);

private:
    QHBoxLayout *m_layout;
    bool m_editMode;
    DImageButton *m_editBtn;
    DImageButton *m_removeBtn;
    QLabel *m_title;
};
}
}

#endif // FINGERITEM_H
