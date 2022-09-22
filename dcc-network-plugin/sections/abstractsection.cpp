// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "abstractsection.h"

#include <QVBoxLayout>
#include <QPaintEvent>
#include <DGuiApplicationHelper>
#include <DStyleOption>

#include <widgets/settingshead.h>
#include <widgets/settingsgroup.h>
#include <widgets/contentwidget.h>

using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

AbstractSection::AbstractSection(QFrame *parent)
    : QFrame(parent)
    , m_group(new SettingsGroup(this, SettingsGroup::GroupBackground))
    , m_layout(new QVBoxLayout(this))
    , m_head(Q_NULLPTR)
{
    m_layout->addWidget(m_group);
    setLayout(m_layout);
}

AbstractSection::AbstractSection(const QString &title, QFrame *parent)
    : AbstractSection(parent)
{
    m_head = new SettingsHead(this);
    m_head->setTitle(title);
    m_head->setEditEnable(false);

    // 需要与列表中文字对齐，列表文字与边框边距为8
    m_head ->setContentsMargins(8, 0, 0, 0);
    m_layout->insertWidget(0, m_head);
}

AbstractSection::~AbstractSection()
{
}

void AbstractSection::setTitle(const QString &title)
{
    if (m_head)
        m_head->setTitle(title);
}

void AbstractSection::appendItem(SettingsItem *item)
{
    m_group->appendItem(item);
}

void AbstractSection::insertItem(int idx, SettingsItem *item)
{
    m_group->insertItem(idx, item);
}

void AbstractSection::removeItem(SettingsItem *item)
{
    m_group->removeItem(item);
}

void AbstractSection::setSettingsHead(SettingsHead *settingsHead)
{
    m_layout->insertWidget(0, settingsHead);
}

int AbstractSection::itemIndex(SettingsItem *item)
{
    for (int i = 0; i < m_group->itemCount(); i++) {
        if (m_group->getItem(i) == item)
            return i;
    }

    return -1;
}
