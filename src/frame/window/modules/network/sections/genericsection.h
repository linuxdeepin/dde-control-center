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

#ifndef GENERICSECTION_H
#define GENERICSECTION_H
#include "interface/namespace.h"

#include "window/modules/network/sections/abstractsection.h"

#include "widgets/lineeditwidget.h"
#include "widgets/switchwidget.h"

#include <networkmanagerqt/connectionsettings.h>

namespace DCC_NAMESPACE {
namespace network {

class GenericSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit GenericSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent = nullptr);
    virtual ~GenericSection() override;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    bool autoConnectChecked() const;
    void setConnectionNameEditable(const bool editable);
    void setConnectionType(NetworkManager::ConnectionSettings::ConnectionType connType);
    inline dcc::widgets::LineEditWidget *connIdItem() { return m_connIdItem; }
    inline dcc::widgets::SwitchWidget *autoConnItem() { return m_autoConnItem; }
    inline bool connectionNameIsEditable() const { return m_connIdItem->isEnabled(); }
    inline const QString connectionName() const { return m_connIdItem->text(); }
    inline void setConnectionName(const QString &name) { m_connIdItem->setText(name); }

private:
    void initUI();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    dcc::widgets::LineEditWidget *m_connIdItem;
    dcc::widgets::SwitchWidget *m_autoConnItem;
    NetworkManager::ConnectionSettings::Ptr m_connSettings;
    NetworkManager::ConnectionSettings::ConnectionType m_connType;
};

} /* network */
} /* dcc */

#endif /* GENERICSECTION_H */
