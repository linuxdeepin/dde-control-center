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

#include "connectwidget.h"
#include <QHBoxLayout>

namespace dcc {
namespace widgets {

ConnectWidget::ConnectWidget(QWidget *parent) :
    QFrame(parent),
    m_disconnectBtn(new DImageButton),
    m_loading(new LoadingIndicator)
{
    m_connectedBtn = new DImageButton;
    m_connectedBtn->setText(tr("Connected"));

    m_loading->setVisible(false);
    m_loading->setFixedSize(22, 22);

    m_disconnectBtn->setVisible(false);
    m_disconnectBtn->setNormalPic(":/display/themes/dark/icons/disconnect.png");
    m_disconnectBtn->setHoverPic(":/display/themes/dark/icons/disconnect_hover.png");
    m_disconnectBtn->setPressPic(":/display/themes/dark/icons/disconnect_press.png");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_loading, 0, Qt::AlignRight);
    mainLayout->addWidget(m_disconnectBtn, 0, Qt::AlignRight);
    mainLayout->addWidget(m_connectedBtn, 0, Qt::AlignRight);

    setLayout(mainLayout);

    connect(m_disconnectBtn, &DImageButton::clicked, this, &ConnectWidget::clicked);
}

void ConnectWidget::onConnectChanged(ConnectState state)
{
    m_state = state;

    switch (state) {
    case Connected:
        m_loading->stop();
        m_loading->setVisible(false);
        m_disconnectBtn->setVisible(false);
        m_connectedBtn->setVisible(true);
        break;
    case ConnectFaild:
        m_loading->stop();
        m_loading->setVisible(false);
        m_disconnectBtn->setVisible(false);
        m_connectedBtn->setVisible(false);
        break;
    case Connecting:
        m_loading->play();
        m_loading->setVisible(true);
        m_disconnectBtn->setVisible(false);
        m_connectedBtn->setVisible(false);
        break;
    default:
        break;
    }
}

void ConnectWidget::setMouseEnter(const bool state)
{
    if (m_state == Connected) {
        if (state) {
            m_disconnectBtn->setVisible(true);
            m_connectedBtn->setVisible(false);
        } else {
            m_disconnectBtn->setVisible(false);
            m_connectedBtn->setVisible(true);
        }
    }
}

}
}
