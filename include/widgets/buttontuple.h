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

#ifndef BUTTONTUPLE_H
#define BUTTONTUPLE_H

#include <DSuggestButton>
#include <DPushButton>
#include <DWarningButton>

#include <QWidget>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {

class LeftButton : public DPushButton
{
    Q_OBJECT
public:
    LeftButton() {}
    virtual ~LeftButton() {}
};

class RightButton : public DSuggestButton
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
    enum ButtonType {
        Normal = 0,
        Save = 1,
        Delete = 2,
    };

    explicit ButtonTuple(ButtonType type = Normal, QWidget *parent = nullptr);

    QPushButton *leftButton();
    QPushButton *rightButton();

    void removeSpacing();

private:
    void createRightButton(const ButtonType type);

Q_SIGNALS:
    void leftButtonClicked();
    void rightButtonClicked();

private:
    QPushButton *m_leftButton;
    QPushButton *m_rightButton;
};

} // namespace widgets
} // namespace dcc

#endif // BUTTONTUPLE_H
