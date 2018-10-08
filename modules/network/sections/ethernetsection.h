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
#include "comboboxwidget.h"
#include "lineeditwidget.h"
#include "switchwidget.h"
#include "spinboxwidget.h"

#include <networkmanagerqt/wiredsetting.h>
#include <networkmanagerqt/wireddevice.h>

namespace dcc {
namespace widgets {

class EthernetSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit EthernetSection(NetworkManager::WiredSetting::Ptr wiredSetting, QFrame *parent = 0);
    virtual ~EthernetSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);

private:
    ComboBoxWidget *m_deviceMac;
    LineEditWidget *m_clonedMac;
    SwitchWidget *m_customMtuSwitch;
    SpinBoxWidget *m_customMtu;

    NetworkManager::WiredSetting::Ptr m_wiredSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
};

} /* widgets */
} /* dcc */

#endif /* ETHERNETSECTION_H */
