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

#ifndef SECRETWIREDSECTION_H
#define SECRETWIREDSECTION_H

#include "secret8021xsection.h"
#include "widgets/switchwidget.h"

namespace dcc {
namespace network {

class SecretWiredSection : public Secret8021xSection
{
    Q_OBJECT

public:
    SecretWiredSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent = 0);
    virtual ~SecretWiredSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    dcc::widgets::SwitchWidget *m_secretEnable;
    Secret8021xEnableWatcher *m_enableWatcher;
};

} /* network */
} /* dcc */

#endif /* SECRETWIREDSECTION_H */
