// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/themeitem.h"
#include "widgets/themeitempic.h"
#include "widgets/basiclistdelegate.h"

#include <QHBoxLayout>
#include <QPixmap>
#include <QMouseEvent>

using namespace dcc;
using namespace dcc::widgets;

ThemeItem::ThemeItem(QWidget *parent)
    : SettingsItem(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_title(new NormalLabel)
    , m_selectLabel(new QLabel)
{
    m_itemPic = new ThemeItemPic;

    m_mainLayout->setMargin(10);
    m_title->setFixedHeight(20);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);

    m_selectLabel->setPixmap(loadPixmap(":/widgets/themes/dark/icons/select.svg"));
    m_selectLabel->setVisible(false);

    titleLayout->addSpacing(10);
    titleLayout->addWidget(m_title, 0, Qt::AlignLeft);
    titleLayout->addWidget(m_selectLabel, 0, Qt::AlignRight);

    m_mainLayout->addLayout(titleLayout);

    m_mainLayout->addWidget(m_itemPic, 0, Qt::AlignHCenter);

    setLayout(m_mainLayout);
}

void ThemeItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void ThemeItem::setSelected(bool selected)
{
    m_selectLabel->setVisible(selected);
    m_state = selected;
}

void ThemeItem::setPic(const QString &picPath)
{
    m_itemPic->setPicPath(picPath);
}

void ThemeItem::setId(const QVariant &id)
{
    m_id = id;
}

void ThemeItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        Q_EMIT selectedChanged(true);
}
