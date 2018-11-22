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

#ifndef MODIFYFULLNAMEPAGE_H
#define MODIFYFULLNAMEPAGE_H

#include "widgets/contentwidget.h"

namespace dcc {

namespace widgets {

class LineEditWidget;

}

namespace accounts {

class User;
class ModifyFullNamePage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyFullNamePage(User *u, QWidget *parent = 0);

Q_SIGNALS:
    void requestSetFullname(User *u, const QString &n) const;

private Q_SLOTS:
    void onFullnameChanged(const QString &fn);

private:
    User *m_user;
    widgets::LineEditWidget *m_fullnameWidget;
};

}

}

#endif // MODIFYFULLNAMEPAGE_H
