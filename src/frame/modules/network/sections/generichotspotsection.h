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

#ifndef GENERICHOTSPOTSECTION_H
#define GENERICHOTSPOTSECTION_H

#include "abstractsection.h"
#include "widgets/lineeditwidget.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace network {

class GenericHotspotSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit GenericHotspotSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent = 0);
    virtual ~GenericHotspotSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();

private:
    dcc::widgets::LineEditWidget *m_connIdItem;

    NetworkManager::ConnectionSettings::Ptr m_connSettings;
};

} /* network */
} /* dcc */

#endif /* GENERICHOTSPOTSECTION_H */
