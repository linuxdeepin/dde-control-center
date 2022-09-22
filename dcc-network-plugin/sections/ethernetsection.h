// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ETHERNETSECTION_H
#define ETHERNETSECTION_H

#include "abstractsection.h"

#include <QComboBox>

#include <networkmanagerqt/wiredsetting.h>
#include <networkmanagerqt/wireddevice.h>

namespace dcc {
  namespace widgets {
    class ComboxWidget;
    class LineEditWidget;
    class SwitchWidget;
    class SpinBoxWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class EthernetSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit EthernetSection(WiredSetting::Ptr wiredSetting, bool deviceAllowEmpty, QString devPath = QString(), QFrame *parent = nullptr);
    virtual ~EthernetSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    QString devicePath() const;

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QComboBox *m_deviceMacComboBox;
    ComboxWidget *m_deviceMacLine;
    LineEditWidget *m_clonedMac;
    SwitchWidget *m_customMtuSwitch;
    SpinBoxWidget *m_customMtu;

    WiredSetting::Ptr m_wiredSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
    QString m_devicePath;
    bool m_deviceAllowEmpty;
};

#endif /* ETHERNETSECTION_H */
