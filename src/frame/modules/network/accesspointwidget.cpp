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

#include "accesspointwidget.h"
#include "widgets/labels/normallabel.h"

#include <QDebug>
#include <QPointer>
#include <QApplication>

#include <dpicturesequenceview.h>
#include <dimagebutton.h>
#include <dhidpihelper.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::network;

static QPointer<DPictureSequenceView> ConnectingIndicator = nullptr;

AccessPointWidget::AccessPointWidget(QWidget *parent)
    : SettingsItem(parent),

      m_lockIcon(new QLabel),
      m_strengthIcon(new QLabel),
      m_apName(new NormalLabel),
      m_activeIcon(new DImageButton),
      m_detailBtn(new DImageButton),

      m_mainLayout(new QHBoxLayout),

      m_strength(-1),
      m_encrypt(false),
      m_connected(false)
{
    m_lockIcon->setFixedSize(16, 16);
    m_strengthIcon->setFixedSize(16, 16);
    m_activeIcon->setFixedSize(16, 16);
    m_activeIcon->setVisible(false);
    m_activeIcon->setNormalPic(":/network/themes/dark/icons/select.svg");
    m_detailBtn->setObjectName("DetailButton");

    m_mainLayout->addWidget(m_lockIcon);
    m_mainLayout->addWidget(m_strengthIcon);
    m_mainLayout->addWidget(m_apName);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_activeIcon);
    m_mainLayout->addWidget(m_detailBtn);
    m_mainLayout->setSpacing(5);
    m_mainLayout->setContentsMargins(20, 5, 10, 5);

    setLayout(m_mainLayout);
    setObjectName("AccessPointWidget");

    connect(m_detailBtn, &DImageButton::clicked, [=] { Q_EMIT requestEdit(m_path, m_apName->text()); });
}

bool AccessPointWidget::operator>(const AccessPointWidget &other) const
{
    return (m_strength / 10 & ~1) > (other.strength() / 10 & ~1);
}

void AccessPointWidget::setEncrypt(const bool encrypt)
{
    m_encrypt = encrypt;

    if (!encrypt)
        m_lockIcon->clear();
    else
        m_lockIcon->setPixmap(DHiDPIHelper::loadNxPixmap(":/network/themes/dark/icons/wireless/security.svg"));
}

void AccessPointWidget::setAPName(const QString &name)
{
    m_apName->setText(name);
}

void AccessPointWidget::setPath(const QString &path)
{
    m_path = path;
}

void AccessPointWidget::setLoadingIndicatorVisible(const bool value)
{
    if (!ConnectingIndicator) {
        const qreal ratio = devicePixelRatioF();
        ConnectingIndicator = new DPictureSequenceView;
        ConnectingIndicator->setMaximumSize(16 * ratio, 16 * ratio);
        ConnectingIndicator->setPictureSequence(":/widgets/themes/dark/icons/Loading/loading_%1.png", QPair<int, int>(0, 90), 3, true);
    }

    if (value) {
        if (m_mainLayout->indexOf(ConnectingIndicator) == -1) {
            m_mainLayout->insertWidget(m_mainLayout->indexOf(m_detailBtn), ConnectingIndicator);
        }
        ConnectingIndicator->setVisible(true);
        ConnectingIndicator->play();
    } else {
        if (m_mainLayout->indexOf(ConnectingIndicator) != -1) {
            ConnectingIndicator->setVisible(false);
            ConnectingIndicator->stop();
            m_mainLayout->removeWidget(ConnectingIndicator);
        }
    }
}

void AccessPointWidget::mouseReleaseEvent(QMouseEvent *e)
{
    SettingsItem::mouseReleaseEvent(e);

    if (!m_connected)
        Q_EMIT requestConnect(m_path, m_apName->text());
}

void AccessPointWidget::setStrength(const int strength)
{
    m_strength = strength;

    if (m_strength < 0)
        m_strength = 0;

    const QString iconName = QString(":/network/themes/dark/icons/wireless/wireless-%1-symbolic.svg").arg(strength / 10 & ~1);
    m_strengthIcon->setPixmap(DHiDPIHelper::loadNxPixmap(iconName));
}

void AccessPointWidget::setConnected(const bool connected)
{
    m_connected = connected;

    if (!m_connected) {
        setLoadingIndicatorVisible(false);
    }

    m_activeIcon->setVisible(m_connected);
}

void AccessPointWidget::setEditable(const bool editable)
{
    m_detailBtn->setVisible(editable);
}
