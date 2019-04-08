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

#include "genericsection.h"

#include <QLineEdit>

using namespace NetworkManager;
using namespace dcc::widgets;
using namespace dcc::network;

GenericSection::GenericSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent)
    : AbstractSection(tr("General"), parent),
      m_connIdItem(new LineEditWidget(this)),
      m_autoConnItem(new SwitchWidget(this)),
      m_connSettings(connSettings)
{
    initUI();
}

GenericSection::~GenericSection()
{
}

bool GenericSection::allInputValid()
{
    bool valid = true;

    valid = !m_connIdItem->textEdit()->text().isEmpty();

    m_connIdItem->setIsErr(!valid);

    return valid;
}

void GenericSection::saveSettings()
{
    m_connSettings->setId(m_connIdItem->text());
    m_connSettings->setAutoconnect(m_autoConnItem->checked());
}

void GenericSection::initUI()
{
    m_connIdItem->setTitle(tr("Name"));
    m_connIdItem->setText(m_connSettings->id());

    m_autoConnItem->setTitle(tr("Auto connect"));
    m_autoConnItem->setChecked(m_connSettings->autoconnect());

    appendItem(m_connIdItem);
    appendItem(m_autoConnItem);

    connect(m_connIdItem->textEdit(), &QLineEdit::editingFinished, this, &GenericSection::allInputValid);
}
