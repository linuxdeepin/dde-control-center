/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liumaochuan <liumaochuan@uniontech.com>
 *
 * Maintainer: liumaochuan <liumaochuan@uniontech.com>
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

#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H
#include <QMouseEvent>
#include <QPushButton>

class custombutton : public QPushButton
{
public:
    custombutton();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:


    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // CUSTOMBUTTON_H
