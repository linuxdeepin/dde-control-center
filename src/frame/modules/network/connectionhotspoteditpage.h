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

#ifndef CONNECTIONHOTSPOTEDITPAGE_H
#define CONNECTIONHOTSPOTEDITPAGE_H

#include "connectioneditpage.h"

namespace dcc {
namespace network {

class ConnectionHotspotEditPage : public ConnectionEditPage
{
    Q_OBJECT

public:
    explicit ConnectionHotspotEditPage(const QString &devPath, const QString &connUuid = QString(), QWidget *parent = 0);
    virtual ~ConnectionHotspotEditPage();

    void initSettingsWidget() Q_DECL_OVERRIDE;
};

} /* network */ 
} /* dcc */ 

#endif /* CONNECTIONHOTSPOTEDITPAGE_H */
