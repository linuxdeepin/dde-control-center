/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#ifndef PASSWDEDITWIDGET_H
#define PASSWDEDITWIDGET_H

#include "widgets/lineeditwidget.h"

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {
class PasswdEditWidget : public LineEditWidget
{
    Q_OBJECT
public:
    explicit PasswdEditWidget(QFrame *parent = nullptr);

    void setSwitchBtnVisible(bool visible);

private Q_SLOTS:
    void switchMode();
    void updateBtnPic();

private:
    DImageButton *m_switchBtn;
};
}
}

#endif // PASSWDEDITWIDGET_H
