// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
