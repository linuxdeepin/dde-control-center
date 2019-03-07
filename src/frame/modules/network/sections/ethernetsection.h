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

#ifndef ETHERNETSECTION_H
#define ETHERNETSECTION_H

#include "abstractsection.h"
#include "widgets/comboboxwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/spinboxwidget.h"

#include <networkmanagerqt/wiredsetting.h>
#include <networkmanagerqt/wireddevice.h>

namespace dcc {
namespace network {

class EthernetSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit EthernetSection(NetworkManager::WiredSetting::Ptr wiredSetting, QFrame *parent = nullptr);
    virtual ~EthernetSection() override;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);

private:
    dcc::widgets::ComboBoxWidget *m_deviceMac;
    dcc::widgets::LineEditWidget *m_clonedMac;
    dcc::widgets::SwitchWidget *m_customMtuSwitch;
    dcc::widgets::SpinBoxWidget *m_customMtu;

    NetworkManager::WiredSetting::Ptr m_wiredSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
};

} /* network */
} /* dcc */

#endif /* ETHERNETSECTION_H */
