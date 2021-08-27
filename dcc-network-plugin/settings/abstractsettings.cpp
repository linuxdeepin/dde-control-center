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

#include "abstractsettings.h"
#include "../sections/genericsection.h"
#include "../sections/abstractsection.h"
#include "../connectioneditpage.h"

#include <QVBoxLayout>
#include <QDebug>

#include <networkmanagerqt/settings.h>

#include "widgets/contentwidget.h"

using namespace NetworkManager;

AbstractSettings::AbstractSettings(ConnectionSettings::Ptr connSettings, QWidget *parent)
    : QWidget(parent)
    , m_connSettings(connSettings)
    , m_sectionsLayout(new QVBoxLayout(this))
{
    m_sectionsLayout->setSpacing(10);
    m_sectionsLayout->setMargin(0);
    m_sectionsLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_sectionsLayout);
}

AbstractSettings::~AbstractSettings()
{
}

bool AbstractSettings::allInputValid()
{
    for (AbstractSection *section : m_settingSections) {
        if (section->isVisible()) {
            if (!section->allInputValid())
                return false;
        }
    }

    return true;
}

void AbstractSettings::saveSettings()
{
    for (auto section : m_settingSections) {
        if (section->isVisible())
            section->saveSettings();
    }

    resetConnectionInterfaceName();
}

bool AbstractSettings::isAutoConnect()
{
    GenericSection *genericSection = dynamic_cast<GenericSection *>(m_settingSections.first());

    if (genericSection)
        return genericSection->autoConnectChecked();

    // auto connect after save connection, like hotspot connection
    return true;
}

void AbstractSettings::resetConnectionInterfaceName()
{
    if (ConnectionEditPage::devicePath().isEmpty() || clearInterfaceName()) {
        m_connSettings->setInterfaceName(QString());
        return;
    }

    Device::Ptr dev = findNetworkInterface(ConnectionEditPage::devicePath());
    if (dev)
        m_connSettings->setInterfaceName(dev->interfaceName());
}
