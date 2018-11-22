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

#include "miracastnodevicewidget.h"
#include "widgets/labels/normallabel.h"
#include "dimagebutton.h"
#include <QVBoxLayout>
#include <QPainter>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::display;
using namespace dcc::widgets;

MiracastNoDeviceWidget::MiracastNoDeviceWidget(QWidget *parent) : TranslucentFrame(parent)
{
    m_loading = new LoadingIndicator;
    m_loading->setVisible(false);

    m_refreshTip = new NormalLabel(tr("Scanning devices, please wait..."));
    m_nodeviceTip = new NormalLabel(tr("No available device found"));

    m_refreshBtn = new DImageButton;
    m_refreshBtn->setText(tr("Refresh"));
    m_refreshBtn->setObjectName("RefreshButton");

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(30);
    layout->addWidget(m_loading, 0, Qt::AlignCenter);
    layout->addWidget(m_refreshTip, 0, Qt::AlignCenter);
    layout->addWidget(m_nodeviceTip, 0, Qt::AlignCenter);
    layout->addWidget(m_refreshBtn, 0, Qt::AlignCenter);
    layout->addSpacing(20);

    setLayout(layout);

    connect(m_refreshBtn, &DImageButton::clicked, this, [=]{
        setState(DeviceListState::Refreshed);
        Q_EMIT requestRefreshed();
    });

    setState(NoDevice);
}

void MiracastNoDeviceWidget::setState(MiracastNoDeviceWidget::DeviceListState state)
{
    switch (state) {
    case Refreshed:
        refresh();
        break;
    case NoDevice:
        nodevice();
        break;
    default:
        break;
    }
}

void dcc::display::MiracastNoDeviceWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 4, 4);

    painter.fillPath(path, QColor(254, 254, 254, 0.13 * 255));
}

void MiracastNoDeviceWidget::refresh()
{
    m_nodeviceTip->setVisible(false);
    m_refreshBtn->setVisible(false);

    m_refreshTip->setVisible(true);
    m_loading->setVisible(true);
    m_loading->play();
}

void MiracastNoDeviceWidget::nodevice()
{
    m_nodeviceTip->setVisible(true);
    m_refreshBtn->setVisible(true);

    m_refreshTip->setVisible(false);
    m_loading->setVisible(false);
    m_loading->stop();
}
