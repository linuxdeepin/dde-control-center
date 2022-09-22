// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNTLSSECTION_H
#define VPNTLSSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
    class FileChooseWidget;
    class SwitchWidget;
  }
}

using namespace NetworkManager;

class VpnTLSSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnTLSSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnTLSSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, QString>> RemoteCertTypeStrMap;

    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    LineEditWidget *m_remote;
    ComboxWidget *m_remoteCertTypeChooser;
    FileChooseWidget *m_caCert;
    SwitchWidget *m_customKeyDirection;
    ComboxWidget *m_keyDirectionChooser;

    QString m_currentRemoteCertType;
    QString m_currentKeyDirection;
};

#endif /* VPNTLSSECTION_H */
