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

#ifndef GENERICSECTION_H
#define GENERICSECTION_H
#include "interface/namespace.h"

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
