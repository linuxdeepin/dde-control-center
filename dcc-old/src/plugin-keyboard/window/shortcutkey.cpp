//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "shortcutkey.h"
#include "keylabel.h"

#include <QMouseEvent>
#include <QDebug>

ShortcutKey::ShortcutKey(QWidget *parent) : QWidget(parent)
{
    setAccessibleName("ShortcutKey");
    m_mainLayout = new QHBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(5);

    setLayout(m_mainLayout);
}

void ShortcutKey::setTextList(const QStringList &list)
{
    for (KeyLabel *label : m_list) {
        m_mainLayout->removeWidget(label);
        label->deleteLater();
    }

    m_list.clear();

    for (const QString &key : list) {
        KeyLabel *label = new KeyLabel(key);
        label->setAccessibleName("LABEL");
        m_list << label;
        m_mainLayout->addWidget(label);
    }

    adjustSize();
}

void ShortcutKey::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);

    for (KeyLabel *label : m_list)
        label->setEnter(true);
}

void ShortcutKey::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    for (KeyLabel *label : m_list)
        label->setEnter(false);
}
