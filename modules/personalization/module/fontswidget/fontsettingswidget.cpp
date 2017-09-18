/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "fontsettingswidget.h"
#include "contentwidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "personalization/personalizationmodel.h"
#include "../../model/fontmodel.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

FontSettingsWidget::FontSettingsWidget(QWidget *parent)
    :TranslucentFrame(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_fontWidget = new SettingsGroup;
    m_standard    = new NextPageWidget;
    m_mono       = new NextPageWidget;

    m_standard->setTitle(tr("Standard Font"));
    m_mono->setTitle(tr("Monospaced Font"));

    m_standard->setAccessibleName("StandardFont");
    m_mono->setAccessibleName("MonospacedFont");

    m_fontWidget->appendItem(m_standard);
    m_fontWidget->appendItem(m_mono);

    m_mainlayout->addWidget(m_fontWidget);
    m_mainlayout->setMargin(0);

    setLayout(m_mainlayout);
    setObjectName("FontSettingsWidget");

    connect(m_standard, &NextPageWidget::clicked, this, &FontSettingsWidget::showStandardFont);
    connect(m_mono, &NextPageWidget::clicked, this, &FontSettingsWidget::showMonoFont);
}

void FontSettingsWidget::setModel(PersonalizationModel * const model)
{
    FontModel *standmodel = model->getStandFontModel();
    FontModel *monomodel  = model->getMonoFontModel();
    connect(standmodel, &FontModel::defaultFontChanged, m_standard, &NextPageWidget::setValue);
    connect(monomodel, &FontModel::defaultFontChanged, m_mono, &NextPageWidget::setValue);

    m_standard->setValue(standmodel->getFontName());
    m_mono->setValue(monomodel->getFontName());
}
