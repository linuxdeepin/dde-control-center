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

#ifndef ABSTRACTSETTINGS_H
#define ABSTRACTSETTINGS_H

#include "contentwidget.h"
#include "../sections/abstractsection.h"

#include <networkmanagerqt/connectionsettings.h>

#include <QWidget>

namespace dcc {
namespace widgets {

class AbstractSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~AbstractSettings();

    bool allInputValid();
    void saveSettings();

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page);

protected:
    virtual void initSections() = 0;

protected:
    NetworkManager::ConnectionSettings::Ptr m_connSettings;

    QVBoxLayout *m_sectionsLayout;
    QList<dcc::widgets::AbstractSection *> m_settingSections;
};

} /* widgets */
} /* dcc */

#endif /* ABSTRACTSETTINGS_H */
