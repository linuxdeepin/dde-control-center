/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#ifndef LOCATIONWIDGET_H
#define LOCATIONWIDGET_H

#include <dtkwidget_global.h>

#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DSwitchButton;
DWIDGET_END_NAMESPACE

class LocationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LocationWidget(QWidget *parent = nullptr);

private:
    void initUI();

private:
    Dtk::Widget::DSwitchButton *m_locatinSwitch; //是否开启定位服务
};

#endif // LOCATIONWIDGET_H
