//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/utils.h"
#include "widgets/accessibleinterface.h"

#include <DBackgroundGroup>
#include <QSizePolicy>

#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(SettingsGroup,"SettingsGroup");
SettingsGroup::SettingsGroup(QFrame *parent, BackgroundStyle bgStyle)
    : QFrame(parent)
    , m_layout(nullptr)
    , m_headerItem(nullptr)
{
    setBackgroundStyle(bgStyle);
}

SettingsGroup::SettingsGroup(const QString &title, QFrame *parent)
    : SettingsGroup(parent)
{
    setHeaderVisible(!title.isEmpty());
    setAccessibleName(title);

    m_headerItem->setTitle(title);
}

SettingsGroup::~SettingsGroup()
{
    if (m_headerItem)
        m_headerItem->deleteLater();
}

void SettingsGroup::setHeaderVisible(const bool visible)
{
    if (visible) {
        if (!m_headerItem)
            m_headerItem = new SettingsHeaderItem;
        insertItem(0, m_headerItem);
    } else {
        if (m_headerItem) {
            m_headerItem->deleteLater();
            m_headerItem = nullptr;
        }
    }
}

void SettingsGroup::insertItem(const int index, SettingsItem *item)
{
    if (ItemBackground == m_bgStyle) {
        //当SettingsItem 被加入　SettingsGroup　时，为其加入背景
        item->addBackground();
    }
    m_layout->insertWidget(index, item);
    item->installEventFilter(this);
}

void SettingsGroup::appendItem(SettingsItem *item)
{
    insertItem(m_layout->count(), item);
}

void SettingsGroup::appendItem(SettingsItem *item, BackgroundStyle bgStyle)
{
    if ((ItemBackground == bgStyle) && (m_bgStyle == ItemBackground)) {
        //当SettingsItem 被加入　SettingsGroup　时，为其加入背景
        item->addBackground();
    }

    m_layout->insertWidget(m_layout->count(), item);
    item->installEventFilter(this);
}

void SettingsGroup::removeItem(SettingsItem *item)
{
    m_layout->removeWidget(item);
    item->removeEventFilter(this);
    item->deleteLater();
}

void SettingsGroup::moveItem(SettingsItem *item, const int index)
{
    const int oldIndex = m_layout->indexOf(item);
    if (oldIndex == index)
        return;

    m_layout->removeWidget(item);
    m_layout->insertWidget(index, item);
}

void SettingsGroup::setSpacing(const int spacing)
{
    m_layout->setSpacing(spacing);
    if (m_bggroup)
        m_bggroup->setItemSpacing(spacing);
}

int SettingsGroup::itemCount() const
{
    return m_layout->count();
}

void SettingsGroup::clear()
{
    const int index = m_headerItem ? 1 : 0;
    const int count = m_layout->count();

    for (int i(index); i != count; ++i) {
        QLayoutItem *item = m_layout->takeAt(index);
        QWidget *w = item->widget();
        w->removeEventFilter(this);
        w->setParent(nullptr);
        delete item;
        w->deleteLater();
    }
}

SettingsItem *SettingsGroup::getItem(int index)
{
    if (index < 0)
        return nullptr;

    if (index < itemCount()) {
        return qobject_cast<SettingsItem *>(m_layout->itemAt(index)->widget());
    }

    return nullptr;
}

void SettingsGroup::insertWidget(QWidget *widget)
{
    m_layout->insertWidget(m_layout->count(), widget);
}

void SettingsGroup::resizeEvent(QResizeEvent *event)
{
    // TODO: maybe here not resize layout, this fixed is not property enough
    QSizePolicy policy;
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);

    QFrame::resizeEvent(event);
}

void SettingsGroup::setBackgroundStyle(BackgroundStyle bgStyle)
{
    if (m_layout)
        delete m_layout;
    if (layout())
        delete layout();


    m_layout = new QVBoxLayout;
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *vLayout = m_layout;
    if (GroupBackground == bgStyle) {
        vLayout = new QVBoxLayout;
        m_bggroup = new DBackgroundGroup(m_layout);
        m_bggroup->setAccessibleName("bggroup");
        m_bggroup->setBackgroundRole(QPalette::Window);
        m_bggroup->setItemSpacing(1);
        m_bggroup->setUseWidgetBackground(false);
        vLayout->addWidget(m_bggroup);
        vLayout->setContentsMargins(0, 0, 0, 0);
    }

    setLayout(vLayout);
    m_bgStyle = bgStyle;
}
