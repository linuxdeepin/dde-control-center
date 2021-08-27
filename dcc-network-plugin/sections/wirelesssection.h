/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

#ifndef WIRELESSSECTION_H
#define WIRELESSSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/wirelessdevice.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
    class SwitchWidget;
    class SpinBoxWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class QComboBox;

class WirelessSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit WirelessSection(WirelessSetting::Ptr wiredSetting, bool isHotSpot = false, QFrame *parent = nullptr);
    virtual ~WirelessSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    void setSsidEditable(const bool editable);
    bool ssidIsEditable() const;
    const QString ssid() const;
    void setSsid(const QString &ssid);

Q_SIGNALS:
    void ssidChanged(const QString &ssid);

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    LineEditWidget *m_apSsid;
    QComboBox *m_deviceMacComboBox;
    ComboxWidget *m_deviceMacLine;
    SwitchWidget *m_customMtuSwitch;
    SpinBoxWidget *m_customMtu;

    WirelessSetting::Ptr m_wirelessSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
};

#endif /* WIRELESSSECTION_H */
