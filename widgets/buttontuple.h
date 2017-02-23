/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BUTTONTUPLE_H
#define BUTTONTUPLE_H

#include <QWidget>
#include <QPushButton>

namespace dcc {
namespace widgets {

class LeftButton : public QPushButton
{
    Q_OBJECT
public:
    LeftButton() {}
    virtual ~LeftButton() {}
};

class RightButton : public QPushButton
{
    Q_OBJECT
public:
    RightButton() {}
    virtual ~RightButton() {}
};


class ButtonTuple : public QWidget
{
    Q_OBJECT
public:
    explicit ButtonTuple(QWidget *parent = 0);

    QPushButton *leftButton();
    QPushButton *rightButton();

signals:
    void leftButtonClicked();
    void rightButtonClicked();

private:
    QPushButton *m_leftButton;
    QPushButton *m_rightButton;
};

} // namespace widgets
} // namespace dcc

#endif // BUTTONTUPLE_H
