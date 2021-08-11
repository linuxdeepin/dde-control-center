/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "notificationitem.h"

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::notification;

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
