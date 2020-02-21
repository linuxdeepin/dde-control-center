/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#pragma once

#include "interface/namespace.h"

#include <QLabel>
#include <QLineEdit>

//以下两个类为了获取控件的点击事件(时间设置,年月日点击QLineEdit和QLabel都能设置焦点到QLineEdit上)
namespace DCC_NAMESPACE {
namespace datetime {

class TimeSetEdit : public QLabel
{
    Q_OBJECT
public:
    explicit TimeSetEdit(QWidget *parent = nullptr);

Q_SIGNALS:
    void notifyClicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

class TimeSetLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit TimeSetLineEdit(QWidget *parent = nullptr);

Q_SIGNALS:
    void notifyClicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
};

}// namespace datetime
}// namespace DCC_NAMESPACE
