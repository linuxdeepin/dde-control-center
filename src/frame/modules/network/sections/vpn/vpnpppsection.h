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

#ifndef VPNPPPSECTION_H
#define VPNPPPSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/comboboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnPPPSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnPPPSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnPPPSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    void setSupportOptions(const QStringList &supportOptions);

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onMppeEnableChanged(const bool checked);
    void onMppeMethodChanged(const QString &method);

private:
    QMap<QString, QString> MppeMethodStrMap;
    QMap<QString, QString> OptionsStrMap;

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    dcc::widgets::SwitchWidget *m_mppeEnable;
    dcc::widgets::ComboBoxWidget *m_mppeChooser;
    dcc::widgets::SwitchWidget *m_mppeStateful;

    QString m_currentMppeMethod;
    QStringList m_supportOptions;
    QList<dcc::widgets::SwitchWidget *> m_optionsWidgets;
};

} /* network */
} /* dcc */

#endif /* VPNPPPSECTION_H */
