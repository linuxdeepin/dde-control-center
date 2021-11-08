/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng_cm@deepin.com>
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

#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QFrame>
#include <QPainter>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMap>

#include "constants.h"

class Button;
class MenuButton;
class CloseButton;
/*!
 * \~chinese \class ActionButton
 * \~chinese \brief
 */
/**
 * @brief The ActionButton class
 * @brief 通知中心中气泡中的功能按钮
 *
 *
 *
 */
class ActionButton : public QFrame
{
    Q_OBJECT

public:
    ActionButton(QWidget *parent = nullptr, OSD::ShowStyle style = OSD::ShowStyle::BUBBLEWINDOW);

    bool addButtons(const QStringList &list);
    QList<Button *> buttonList() { return m_buttons; }
    bool isEmpty();
    void clear();

    QSize buttonSize() const {return m_buttonSize;}
    void setButtonSize(const QSize &size);
    QSize contentSize(const QString &text, bool is_menu = false) const;

private:
    void initUI();
    void initConnections();

Q_SIGNALS:
    void buttonClicked(const QString &id);

private:
    OSD::ShowStyle m_showStyle;
    QHBoxLayout *m_layout = nullptr;
    QList<Button *> m_buttons;
    Button *m_menuButton = nullptr;

    QSize m_buttonSize = QSize(70, 60);
};

#endif // ACTIONBUTTON_H
