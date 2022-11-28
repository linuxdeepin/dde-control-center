// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GENERICSECTION_H
#define GENERICSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class SwitchWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class GenericSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit GenericSection(ConnectionSettings::Ptr connSettings, QFrame *parent = nullptr);
    virtual ~GenericSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    bool autoConnectChecked() const;
    void setConnectionNameEditable(const bool editable);
    void setConnectionType(ConnectionSettings::ConnectionType connType);
    inline LineEditWidget *connIdItem() { return m_connIdItem; }
    inline SwitchWidget *autoConnItem() { return m_autoConnItem; }
    bool connectionNameIsEditable();
    const QString connectionName() const;
    void setConnectionName(const QString &name);

Q_SIGNALS:
    void ssidChanged(const QString &ssid);

private:
    void initUI();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    LineEditWidget *m_connIdItem;
    SwitchWidget *m_autoConnItem;
    ConnectionSettings::Ptr m_connSettings;
    ConnectionSettings::ConnectionType m_connType;
};

#endif /* GENERICSECTION_H */
