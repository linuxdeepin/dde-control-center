// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SECRETHOTSPOTSECTION_H
#define SECRETHOTSPOTSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/wirelesssecuritysetting.h>

namespace dcc {
  namespace widgets {
    class ComboxWidget;
    class LineEditWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;

class SecretHotspotSection : public AbstractSection
{
    Q_OBJECT

public:
    SecretHotspotSection(WirelessSecuritySetting::Ptr wsSeting, QFrame *parent = nullptr);
    virtual ~SecretHotspotSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onKeyMgmtChanged(WirelessSecuritySetting::KeyMgmt);
    void saveUserInputPassword();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QMap<QString, WirelessSecuritySetting::KeyMgmt> KeyMgmtStrMap;

    ComboxWidget *m_keyMgmtChooser;
    PasswdLineEditWidget *m_passwdEdit;

    WirelessSecuritySetting::KeyMgmt m_currentKeyMgmt;

    WirelessSecuritySetting::Ptr m_wsSetting;

    QMap<WirelessSecuritySetting::KeyMgmt, QString> m_userInputPasswordMap;
};

#include "declare_metatype_for_networkmanager.h"

#endif /* SECRETHOTSPOTSECTION_H */
