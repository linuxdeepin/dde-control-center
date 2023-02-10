//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "notificationitem.h"

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>

using namespace DCC_NAMESPACE;

NotificationItem::NotificationItem(QWidget *parent)
    : SettingsItem(parent)
    , m_layout(new QHBoxLayout)
    , m_chkState(new QCheckBox)
{
    setFixedHeight(38);

    m_chkState->setAccessibleName("QCheckBox");
    m_layout->setContentsMargins(10, 0, 10, 0);
    m_layout->addWidget(m_chkState);
    m_layout->addStretch();

    setLayout(m_layout);

    connect(m_chkState, &QCheckBox::stateChanged, [this]() {
        Q_EMIT stateChanged(getState());
    });
}

void NotificationItem::setTitle(const QString &title)
{
    if (title.isEmpty()) {
        m_chkState->setText(nullptr);
    } else {
        m_chkState->setText(title);
    }
}

bool NotificationItem::getState() const
{
    return m_chkState->isChecked();
}

void NotificationItem::setState(const bool &state)
{
    if (state != getState()) {
        m_chkState->setChecked(state);
        Q_EMIT stateChanged(state);
    }
}
