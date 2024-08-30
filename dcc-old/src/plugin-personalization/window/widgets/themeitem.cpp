//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "themeitem.h"
#include "themeitempic.h"

#include <DIconButton>

#include <QVBoxLayout>
#include <QLabel>


DWIDGET_USE_NAMESPACE

ThemeItem::ThemeItem(bool titleBelowPic, QWidget *parent)
    : QWidget(parent)
    , m_titleBelowPic(titleBelowPic)
    , m_imgBtn(nullptr)
{
    m_mainLayout = new  QVBoxLayout();
    m_mainLayout->setMargin(0);

    m_title = new QLabel(this);
    m_itemPic = new ThemeItemPic(this);
    connect(m_itemPic, &ThemeItemPic::clicked, this, [=] {
        Q_EMIT selectedChanged(true);
    });

    if (m_titleBelowPic) {
        m_mainLayout->addWidget(m_itemPic);
        m_mainLayout->addWidget(m_title);
        m_mainLayout->setAlignment(Qt::AlignHCenter);
    } else {
        //icon themes and cursor thems
        QHBoxLayout *titlebuttonLayout = new QHBoxLayout();
        titlebuttonLayout->addWidget(m_title);
        m_imgBtn = new DIconButton(DStyle::SP_MarkElement, this);
        m_imgBtn->setDisabled(true);
        titlebuttonLayout->addStretch();
        titlebuttonLayout->addWidget(m_imgBtn);

        m_mainLayout->addLayout(titlebuttonLayout);
        m_mainLayout->addWidget(m_itemPic);
    }

    m_mainLayout->setSpacing(5);

    setLayout(m_mainLayout);
}

void ThemeItem::setTitle(const QString &title)
{
    m_title->setText(title);
    m_itemPic->setAccessibleName(title);
    m_mainLayout->setAlignment(m_title, Qt::AlignCenter);
}

void ThemeItem::setSelected(bool selected)
{
    m_state = selected;
    if (m_titleBelowPic) {
        m_itemPic->setSelected(selected);
    } else {
        m_imgBtn->setVisible(selected);
    }
}

void ThemeItem::setPic(const QString &picPath)
{
    m_itemPic->setPath(picPath);
    m_mainLayout->setAlignment(m_title, Qt::AlignCenter);
}

void ThemeItem::setId(const QVariant &id)
{
    m_id = id;
}
