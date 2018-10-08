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

#include "abstractsettings.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::network;

AbstractSettings::AbstractSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent)
    : QWidget(parent),
      m_connSettings(connSettings),
      m_sectionsLayout(new QVBoxLayout)
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
    for (auto section : m_settingSections) {
        if (section->isVisible()) {
            if (!section->allInputValid()) {
                qDebug() << "some section has Error!" << m_settingSections.indexOf(section);
                return false;
            }
        }
    }

    return true;
}

void AbstractSettings::saveSettings()
{
    for (auto section : m_settingSections) {
        if (section->isVisible()) {
            section->saveSettings();
        }
    }
}
