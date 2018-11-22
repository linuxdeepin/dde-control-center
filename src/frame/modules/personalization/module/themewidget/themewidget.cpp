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

#include "themewidget.h"
#include "widgets/settingsgroup.h"
#include "../../personalizationmodel.h"
#include "theme.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

ThemeWidget::ThemeWidget(QWidget *parent)
    :ContentWidget(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_widget = new TranslucentFrame;
    m_windowTheme = new Theme(tr("Window Theme"));
    m_iconTheme = new Theme(tr("Icon Theme"));
    m_mouseTheme = new Theme(tr("Cursor Theme"));

    m_mainlayout->addSpacing(10);

    m_mainlayout->addWidget(m_windowTheme);
    m_mainlayout->addWidget(m_iconTheme);
    m_mainlayout->addWidget(m_mouseTheme);

    m_mainlayout->setMargin(0);
    m_mainlayout->setSpacing(20);

    m_widget->setLayout(m_mainlayout);
    setTitle(tr("Theme"));
    setContent(m_widget);

    connect(m_windowTheme, &Theme::requestSetDefault, this, &ThemeWidget::requestSetDefault);
    connect(m_iconTheme,   &Theme::requestSetDefault, this, &ThemeWidget::requestSetDefault);
    connect(m_mouseTheme,  &Theme::requestSetDefault, this, &ThemeWidget::requestSetDefault);
}

void ThemeWidget::setModel(PersonalizationModel * const model)
{
    m_windowTheme->setModel(model->getWindowModel());
    m_iconTheme->setModel(model->getIconModel());
    m_mouseTheme->setModel(model->getMouseModel());
}
