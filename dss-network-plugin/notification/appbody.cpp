/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#include "appbody.h"
#include "appbodylabel.h"
#include "constants.h"

#include <QPainter>
#include <QDebug>
#include <QVBoxLayout>

#include <DGuiApplicationHelper>
#include <DFontSizeManager>

DGUI_USE_NAMESPACE

AppBody::AppBody(QWidget *parent)
    : QFrame(parent)
    , m_titleLbl(new AppBodyLabel(this))
    , m_bodyLbl(new AppBodyLabel(this))
    , m_showStyle(OSD::BUBBLEWINDOW)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, BubbleAppBodyPaddingTop, 0, BubbleAppBodyPaddingBottom);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(m_titleLbl, 0, Qt::AlignVCenter);
    layout->addWidget(m_bodyLbl, 0, Qt::AlignVCenter);
    layout->addStretch();

    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &AppBody::refreshTheme);
    refreshTheme();
}

void AppBody::setTitle(const QString &title)
{
    m_titleLbl->setFont(DFontSizeManager::instance()->t6());
    m_titleLbl->setText(title);
    m_titleLbl->setVisible(!title.isEmpty());
}

void AppBody::setText(const QString &text)
{
    m_bodyLbl->setFont(DFontSizeManager::instance()->t7());
    m_bodyLbl->setText(text);
    m_bodyLbl->setVisible(!text.isEmpty());
}

void AppBody::setStyle(OSD::ShowStyle style)
{
    m_showStyle = style;
    refreshTheme();
}

int AppBody::bubbleWidgetAppBodyHeight()
{
    int titleLabelHeight = QFontMetrics(DFontSizeManager::instance()->t6()).height();
    int bodyLabelHeight = QFontMetrics(DFontSizeManager::instance()->t7()).height();
    return titleLabelHeight + bodyLabelHeight + BubbleAppBodyVerticalPadding;
}

int AppBody::bubbleWindowAppBodyHeight()
{
    return fontMetrics().height() * 2 + BubbleAppBodyVerticalPadding;
}

void AppBody::refreshTheme()
{
    m_titleLbl->setForegroundRole(QPalette::BrightText);
    m_bodyLbl->setForegroundRole(QPalette::BrightText);

    if (m_showStyle == OSD::BUBBLEWIDGET) {
        m_titleLbl->setOpacity(1.0);
        m_bodyLbl->setOpacity(Notify::BubbleOpacity);

        m_titleLbl->setFont(DFontSizeManager::instance()->t6());
        m_bodyLbl->setFont(DFontSizeManager::instance()->t7());
    } else {
        m_titleLbl->setOpacity(Notify::BubbleOpacity);
        m_bodyLbl->setOpacity(1.0);
    }
}
