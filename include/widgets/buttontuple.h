//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
