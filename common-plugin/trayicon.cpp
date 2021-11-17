/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "trayicon.h"
#include "networkpanel.h"
#include "dde-dock/constants.h"

#include <QMouseEvent>
#include <QPainter>

TrayIcon::TrayIcon(NetworkPanel *panel)
    : QWidget()
    , m_panel(panel)
{
    m_panel->setMainWidget(this);
    setAccessibleName(QStringLiteral("NetworkTrayIcon"));
    connect(m_panel, &NetworkPanel::iconChange, this, &TrayIcon::updateIcon);
    setFixedSize(QSize(20, 20));
    setBackgroundRole(DPalette::Button);
}

void TrayIcon::updateIcon()
{
    m_iconPixmap = m_panel->icon();
    update();
}

void TrayIcon::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    const QRectF &rf = rect();
    const QRectF &rfp = QRectF(m_iconPixmap.rect());
    painter.drawPixmap(rf.center() - rfp.center() / m_iconPixmap.devicePixelRatioF(),
                       m_iconPixmap);
}

void TrayIcon::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    // 保持横纵比
    if (position == Dock::Bottom || position == Dock::Top) {
        setMaximumWidth(height());
        setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        setMaximumHeight(width());
        setMaximumWidth(QWIDGETSIZE_MAX);
    }

    m_panel->refreshIcon();
}

void TrayIcon::showNetworkDialog()
{
    if (isVisible()) {
        emit signalShowNetworkDialog(this);
    }
}
