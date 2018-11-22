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

#include "fontswidget.h"
#include "fontsizewidget.h"
#include "fontsettingswidget.h"
#include "widgets/nextpagewidget.h"
#include "../../personalizationmodel.h"
#include "../../model/fontmodel.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

FontsWidget::FontsWidget(QWidget *parent)
    :ContentWidget(parent)
{
    m_mainlayout         = new QVBoxLayout;
    m_widget             = new TranslucentFrame;
    m_fontSizeWidget     = new FontSizeWidget;
    m_fontSettingsWidget = new FontSettingsWidget;

    m_mainlayout->addSpacing(10);
    m_mainlayout->addWidget(m_fontSizeWidget);
    m_mainlayout->addWidget(m_fontSettingsWidget);
    m_mainlayout->setMargin(0);
    m_mainlayout->setSpacing(10);

    m_widget->setLayout(m_mainlayout);
    setTitle(tr("Font"));
    setContent(m_widget);

    connect(m_fontSettingsWidget, &FontSettingsWidget::showStandardFont, this, &FontsWidget::showStandardFont);
    connect(m_fontSettingsWidget, &FontSettingsWidget::showMonoFont,    this, &FontsWidget::showMonoFont);
    connect(m_fontSizeWidget,     &FontSizeWidget::requestSetFontSize,  this, &FontsWidget::requestSetFontSize);
}

void FontsWidget::setModel(PersonalizationModel *const model)
{
    m_fontSizeWidget->setModel(model->getFontSizeModel());
    m_fontSettingsWidget->setModel(model);
}
