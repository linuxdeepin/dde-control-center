/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xuyanghe <xuyanghe@uniontech.com>
   *
   * Maintainer: xuyanghe <xuyanghe@uniontech.com>
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

#include "interface/namespace.h"

#include <DWidget>

#include <QIcon>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {
/*!
 * \~chinese \class CustomButton
 * \~chinese \brief 自定义一个控件,继承于DWidget
 */
class CustomButton : public DWidget
{
    Q_OBJECT
public:
    explicit CustomButton(QWidget *parent = nullptr);
    /*!
     * \~chinese \name setText
     * \~chinese \brief 设置按钮上显示的文字
     * \~chinese \param text 按钮上需要显示的文字
     */
    void setText(const QString &text);
    /*!
     * \~chinese \name setIcon
     * \~chinese \brief 设置按钮上显示的图片
     * \~chinese \param text 按钮上需要显示的图片
     */
    void setIcon(const QIcon &icon);
    /*!
     * \~chinese \name setDueModel
     * \~chinese \brief 设置是否为平板模式
     * \~chinese \param dueModel 是平板模式
     */
    void setDueModel(bool dueModel);
    /*!
     * \~chinese \name setBackOpacity
     * \~chinese \brief 设置控件背景不透明度
     * \~chinese \param opacity 不透明度,取值范围为(0-255)
     */
    void setBackOpacity(int opacity);
    /*!
     * \~chinese \name setRadius
     * \~chinese \brief 设置控件圆角半径
     * \~chinese \param radius 半径,当输入的半径<=0时控件显示为直角
     */
    void setRadius(int radius);

Q_SIGNALS:
    void clicked();

private:
    QString m_text;
    QIcon m_icon;
    bool m_dueModel;
    bool m_hover;
    int m_opacity;
    int m_radius;
    bool m_hasBackColor;

protected:
    /*!
     * \~chinese \name paintEvent
     * \~chinese \brief 绘制控件的样式
     * \~chinese \param event 事件
     */
    virtual void paintEvent(QPaintEvent *event) override;
    /*!
     * \~chinese \name mousePressEvent
     * \~chinese \brief 鼠标点击事件
     * \~chinese \param event 事件
     */
    // TODO 鼠标事件就不要传递给父对象了，包括mouseReleaseEvent
    virtual void mousePressEvent(QMouseEvent *event) override;
    /*!
     * \~chinese \name enterEvent
     * \~chinese \brief 鼠标移动到控件上产生的事件,产生悬停的效果
     * \~chinese \param event 事件
     */
    virtual void enterEvent(QEvent *event) override;
    /*!
     * \~chinese \name leaveEvent
     * \~chinese \brief 鼠标移出控件产生的事件,消除悬停的效果
     * \~chinese \param event 事件
     */
    virtual void leaveEvent(QEvent *event) override;
};
}
}

#endif // CUSTOMBUTTON_H
