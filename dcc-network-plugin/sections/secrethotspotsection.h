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
    LineEditWidget *m_passwdEdit;

    WirelessSecuritySetting::KeyMgmt m_currentKeyMgmt;

    WirelessSecuritySetting::Ptr m_wsSetting;

    QMap<WirelessSecuritySetting::KeyMgmt, QString> m_userInputPasswordMap;
};

#include "declare_metatype_for_networkmanager.h"

#endif /* SECRETHOTSPOTSECTION_H */
