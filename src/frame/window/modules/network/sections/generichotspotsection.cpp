/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "generichotspotsection.h"

#include <QLineEdit>

using namespace NetworkManager;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;

GenericHotspotSection::GenericHotspotSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection(tr("General"), parent)
    , m_connIdItem(new LineEditWidget(this))
    , m_connSettings(connSettings)
{
    initUI();
}

GenericHotspotSection::~GenericHotspotSection()
{
}

bool GenericHotspotSection::allInputValid()
{
    bool valid = true;

    valid = !m_connIdItem->textEdit()->text().isEmpty();

    m_connIdItem->setIsErr(!valid);

    return valid;
}

void GenericHotspotSection::saveSettings()
{
    m_connSettings->setId(m_connIdItem->text());

    // hotspot connection should not be activated automatically by daemon
    m_connSettings->setAutoconnect(false);
}

void GenericHotspotSection::initUI()
{
    m_connIdItem->setTitle(tr("Name"));
    m_connIdItem->setText(m_connSettings->id());

    appendItem(m_connIdItem);

    m_connIdItem->textEdit()->installEventFilter(this);
}

bool GenericHotspotSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if ((dynamic_cast<QLineEdit*>(watched))) {
            Q_EMIT editClicked();
        }
    }
    return QWidget::eventFilter(watched, event);
}
