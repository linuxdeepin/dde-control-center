/*
 * Copyright (C) 2016 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             guoyao <guoyao@uniontech.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             guoyao <guoyao@uniontech.com>
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

#include <interface/namespace.h>
#include <QWidget>

class QPushButton;

namespace DCC_NAMESPACE
{
/**
 * @brief ButtonTuple 提供一个按钮组合，可以通过leftButton和rightButton来访问按钮对象
 */
class ButtonTuple : public QWidget
{
    Q_OBJECT
public:
    enum ButtonType {
        Normal = 0, // 默认按钮，无任何特殊处理
        Save = 1,   // 使用DSuggestButton实现
        Delete = 2, // 使用DWarningButton实现
    };
    explicit ButtonTuple(ButtonType type = Normal, QWidget *parent = nullptr);

    void setButtonType(const ButtonType type);
    QPushButton *leftButton();
    QPushButton *rightButton();

    /**
     * @brief 删除两个按钮间的空隙
     */
    void removeSpacing();

private:
    void initUI();

Q_SIGNALS:
    void leftButtonClicked();
    void rightButtonClicked();

private:
    QPushButton *m_leftButton;
    QPushButton *m_rightButton;
};

} // namespace DCC_NAMESPACE
