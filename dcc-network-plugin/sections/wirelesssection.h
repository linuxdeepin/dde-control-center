// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WIRELESSSECTION_H
#define WIRELESSSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/connectionsettings.h>

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
    explicit WirelessSection(ConnectionSettings::Ptr connSettings, WirelessSetting::Ptr wiredSetting, QString devPath, bool isHotSpot = false, QFrame *parent = nullptr);
    virtual ~WirelessSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    void setSsid(const QString &ssid);

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);

private:
    QString m_ssid;
    QComboBox *m_deviceMacComboBox;
    ComboxWidget *m_deviceMacLine;
    SwitchWidget *m_customMtuSwitch;
    SpinBoxWidget *m_customMtu;

    ConnectionSettings::Ptr m_connSettings;
    WirelessSetting::Ptr m_wirelessSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QPair<QString, QString>> m_macStrMap;
};

#endif /* WIRELESSSECTION_H */
