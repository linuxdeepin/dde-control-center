/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "miracastitem.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>

#include <QDebug>

using namespace dcc;
using namespace dcc::display;
using namespace dcc::widgets;

MiracastItem::MiracastItem(const SinkInfo &info, QWidget *parent) :
    SettingsItem(parent),
    m_info(info)
{
    QLabel *title = new QLabel(m_info.m_name);
    m_connectWidget = new ConnectWidget;

    if (m_info.m_connected)
        m_connectWidget->onConnectChanged(ConnectWidget::Connected);
    else
        m_connectWidget->onConnectChanged(ConnectWidget::ConnectFaild);

    QHBoxLayout *mainlayout = new QHBoxLayout;
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    mainlayout->setContentsMargins(20, 5, 10, 5);
    mainlayout->addWidget(title);
    mainlayout->addStretch();
    mainlayout->addWidget(m_connectWidget, 0, Qt::AlignRight);
    setLayout(mainlayout);
    setFixedHeight(36);

    connect(m_connectWidget, &ConnectWidget::clicked, this, &MiracastItem::onDisConnect);
}

const SinkInfo MiracastItem::info() const
{
   return m_info;
}

void MiracastItem::onConnectState(bool state)
{
    m_info.m_connected = state;

    if (state)
        m_connectWidget->onConnectChanged(ConnectWidget::Connected);
    else
        m_connectWidget->onConnectChanged(ConnectWidget::ConnectFaild);
}

void MiracastItem::onDisConnect()
{
    Q_EMIT requestSinkDisConnect(m_info.m_sinkPath);
}

void MiracastItem::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    if (!m_info.m_connected) {
        m_connectWidget->onConnectChanged(ConnectWidget::Connecting);
        Q_EMIT requestSinkConnect(m_info.m_sinkPath, QApplication::primaryScreen()->availableGeometry());
    }
}

void MiracastItem::enterEvent(QEvent *e)
{
    SettingsItem::enterEvent(e);

    m_connectWidget->setMouseEnter(true);
}

void MiracastItem::leaveEvent(QEvent *e)
{
    SettingsItem::leaveEvent(e);

    m_connectWidget->setMouseEnter(false);
}
