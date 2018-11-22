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

#include "secretwiredsection.h"

using namespace dcc::network;
using namespace dcc::widgets;

SecretWiredSection::SecretWiredSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent)
    : Secret8021xSection(sSetting, parent),
      m_secretEnable(new SwitchWidget(this)),
      m_enableWatcher(new Secret8021xEnableWatcher(this))
{
    m_secretEnable->setTitle(tr("Security Required"));
    m_secretEnable->setChecked(!sSetting->toMap().isEmpty());

    m_enableWatcher->setSecretEnable(m_secretEnable->checked());

    connect(m_secretEnable, &SwitchWidget::checkedChanged, m_enableWatcher, &Secret8021xEnableWatcher::setSecretEnable);

    QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsWantedList;
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodMd5);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodFast);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTtls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodPeap);

    init(m_enableWatcher, eapMethodsWantedList);

    insertItem(1, m_secretEnable);
}

SecretWiredSection::~SecretWiredSection()
{
}

bool SecretWiredSection::allInputValid()
{
    return Secret8021xSection::allInputValid();
}

void SecretWiredSection::saveSettings()
{
    Secret8021xSection::saveSettings();
}
