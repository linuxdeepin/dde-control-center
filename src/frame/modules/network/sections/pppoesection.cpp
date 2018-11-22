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

#include "pppoesection.h"

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

PPPOESection::PPPOESection(NetworkManager::PppoeSetting::Ptr pppoeSetting, QFrame *parent)
    : AbstractSection(tr("PPPoE"), parent),
      m_pppoeSetting(pppoeSetting),
      m_userName(new LineEditWidget(this)),
      m_service(new LineEditWidget(this)),
      m_password(new PasswdEditWidget(this))
{
    initUI();
    initConnection();
}

PPPOESection::~PPPOESection()
{
}

bool PPPOESection::allInputValid()
{
    bool valid = true;

    if (m_userName->text().isEmpty()) {
        valid = false;
        m_userName->setIsErr(true);
    } else {
        m_userName->setIsErr(false);
    }

    if (m_password->text().isEmpty()) {
        valid = false;
        m_password->setIsErr(true);
    } else {
        m_password->setIsErr(false);
    }

    return valid;
}

void PPPOESection::saveSettings()
{
    m_pppoeSetting->setUsername(m_userName->text());
    m_pppoeSetting->setService(m_service->text());
    m_pppoeSetting->setPasswordFlags(NetworkManager::Setting::SecretFlagType::None);
    m_pppoeSetting->setPassword(m_password->text());

    m_pppoeSetting->setInitialized(true);
}

void PPPOESection::initUI()
{
    m_userName->setTitle(tr("Username"));
    m_userName->setPlaceholderText(tr("Required"));
    m_userName->setText(m_pppoeSetting->username());

    m_service->setTitle(tr("Service"));
    m_service->setText(m_pppoeSetting->service());

    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Required"));
    m_password->setText(m_pppoeSetting->password());

    appendItem(m_userName);
    appendItem(m_service);
    appendItem(m_password);
}

void PPPOESection::initConnection()
{
    connect(m_userName->textEdit(), &QLineEdit::editingFinished, this, &PPPOESection::allInputValid);
    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, &PPPOESection::allInputValid);
}
