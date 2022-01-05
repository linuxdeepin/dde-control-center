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
#include <QAction>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QtMath>
#include <QDebug>
#include <QStyle>
#include <QMenu>
#include <QGridLayout>
#include <DStyleHelper>
#include <QApplication>

#include "actionbutton.h"
#include "button.h"
#include "constants.h"

ActionButton::ActionButton(QWidget *parent, OSD::ShowStyle style) :
    QFrame(parent)
    , m_showStyle(style)
    , m_menuButton(new Button())
{
    initUI();
    initConnections();
}

bool ActionButton::addButtons(const QStringList &list)
{
    if (list.isEmpty()) {
        return true;
    }
    // Each even element in the list (starting at index 0) represents the
    // identifier for the action. Each odd element in the list is the
    // localized string that will be displayed to the user.
    QString id;

    for (int i = 0; i != list.size(); ++i) {
        //先获取action ID
        if (i % 2 == 0) {
            id = list[i];
        } else {
            if (i == 1) {
                DStyleHelper dstyle(style());
                const int round = dstyle.pixelMetric(DStyle::PM_FrameRadius);
                Button *button = new Button();
                button->setAccessibleName("Button");
                button->setText(list[i]);
                button->setRadius(round);
                button->setFixedSize(contentSize(list[i]));

                m_layout->addWidget(button);
                //避免数据大量接受引起的按钮隐藏
                button->show();

                connect(button, &Button::clicked, this, [ = ] {
                    Q_EMIT buttonClicked(id);
                });

                m_buttons << button;
            } else if (i == 3) {
                m_menuButton->setText(list[i]);
                m_menuButton->setId(id);
                m_menuButton->setFixedSize(contentSize(list[i], true));

                m_buttons << m_menuButton;
            } else {
                QAction *action = new QAction(list[i]);

                connect(action, &QAction::triggered, this, [ = ] {
                    Q_EMIT buttonClicked(id);
                });

                m_menuButton->addAction(action);
            }
        }
    }

    if (list.size() > 2) {
        m_menuButton->show();
    }

    return true;
}

bool ActionButton::isEmpty()
{
    return m_buttons.isEmpty();
}

void ActionButton::clear()
{
    for (Button *button : m_buttons) {
        button->clear();
        button->hide();
        m_layout->removeWidget(button);
        if (button != m_menuButton)
            button->deleteLater();
    }
    m_buttons.clear();
}

void ActionButton::setButtonSize(const QSize &size)
{
    m_buttonSize = size;
    m_menuButton->setFixedSize(size);
}

void ActionButton::initUI()
{
    DStyleHelper dstyle(style());
    const int round = dstyle.pixelMetric(DStyle::PM_FrameRadius);
    m_menuButton->setAccessibleName("MenuButton");
    m_menuButton->setRadius(round);
    m_menuButton->hide();

    m_layout = new QHBoxLayout;
    m_layout->setMargin(0);
    m_layout->setSpacing(10);
    m_layout->setAlignment(Qt::AlignRight);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(10);
    layout->setMargin(0);
    layout->addLayout(m_layout);
    layout->addWidget(m_menuButton);

    setLayout(layout);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

QSize ActionButton::contentSize(const QString &text, bool is_menu) const
{
    const QFont appNamefont(qApp->font());
    const QFontMetrics fm(appNamefont);

    if (m_showStyle == OSD::BUBBLEWINDOW) {
        int text_width = fm.width(text) + 15;
        int max_width = qMax(text_width, OSD::ButtonSize(m_showStyle).width());
        if (is_menu) {
            max_width = qMax(MenuWidth + 2 * MenuPadding + text_width, OSD::ButtonSize(m_showStyle).width());
        }
        return QSize(qMin(max_width, MaxBubbleButtonWidth), OSD::ButtonSize(m_showStyle).height());
    }

    return OSD::ButtonSize(m_showStyle);
}

void ActionButton::initConnections()
{
    connect(m_menuButton, &Button::toggled, this, &ActionButton::buttonClicked);
}
