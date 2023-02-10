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

#ifndef TITLEBUTTONITEM_H
#define TITLEBUTTONITEM_H

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class TitleButtonItem : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    explicit TitleButtonItem(QFrame *parent = nullptr);
    void setTitle(const QString &title);
    void setValue(const QString &value);

Q_SIGNALS:
    void click();

private:
    void updateTitleSize();

private:
    QLabel *m_title;
    QPushButton *m_button;
};

#endif // TITLEBUTTONITEM_H
