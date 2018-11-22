/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "defappwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include <QPushButton>
#include <QDebug>

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;
DefaultAppsWidget::DefaultAppsWidget()
    : ModuleWidget(),

    m_userGroup(new SettingsGroup)
{
    setObjectName("Defapp");

    m_centralLayout->addWidget(m_userGroup);

    m_nextPage = new NextPageWidget;
    m_nextPage->setTitle(tr("Default Applications"));
    m_userGroup->appendItem(m_nextPage);
    m_centralLayout->addStretch(0);

    setTitle(tr("Default Applications"));
    connect(m_nextPage, &NextPageWidget::clicked, this, &DefaultAppsWidget::showDefaultAppsDetail);
}
