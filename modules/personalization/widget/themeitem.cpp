/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "themeitem.h"
#include "themeitempic.h"
#include <QHBoxLayout>
#include <QPixmap>
#include <QMouseEvent>

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

ThemeItem::ThemeItem(const QJsonObject &json):
    m_mainLayout(new QVBoxLayout),
    m_title(new NormalLabel),
    m_selectLabel(new QLabel)
{
    m_itemPic = new ThemeItemPic;

    m_mainLayout->setMargin(10);
    m_title->setFixedHeight(20);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);

    m_selectLabel->setPixmap(QPixmap(":/defapp/icons/select.png"));
    m_selectLabel->setVisible(false);

    titleLayout->addSpacing(10);
    titleLayout->addWidget(m_title, 0, Qt::AlignLeft);
    titleLayout->addWidget(m_selectLabel, 0, Qt::AlignRight);

    m_mainLayout->addLayout(titleLayout);

    m_mainLayout->addWidget(m_itemPic, 0, Qt::AlignHCenter);

    setLayout(m_mainLayout);
    setAccessibleName(json["Id"].toString());

    setTitle(json["Id"].toString());
}

void ThemeItem::setTitle(const QString &title)
{
    m_id = title;
    QString t = title == "deepin" ? "deepin ("+tr("Default") + ")" : title;
    m_title->setText(t);
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

const QString ThemeItem::id() const
{
    return m_id;
}

void ThemeItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        emit selectedChanged(true);
}
