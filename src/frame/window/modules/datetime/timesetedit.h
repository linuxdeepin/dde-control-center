// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
