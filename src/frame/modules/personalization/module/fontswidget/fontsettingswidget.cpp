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

#include "fontsettingswidget.h"
#include "widgets/contentwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "../../personalizationmodel.h"
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
    m_model = model;

    FontModel *standmodel = model->getStandFontModel();
    FontModel *monomodel  = model->getMonoFontModel();
    connect(standmodel, &FontModel::defaultFontChanged, this, &FontSettingsWidget::onStandFontChanged);
    connect(monomodel, &FontModel::defaultFontChanged, this, &FontSettingsWidget::onMonoFontChanged);
    connect(standmodel, &FontModel::listChanged, this, [=] {
        onStandFontChanged(standmodel->getFontName());
    });
    connect(monomodel, &FontModel::listChanged, this, [=] {
        onMonoFontChanged(monomodel->getFontName());
    });

    onStandFontChanged(standmodel->getFontName());
    onMonoFontChanged(monomodel->getFontName());
}

void FontSettingsWidget::onStandFontChanged(const QString &name)
{
    FontModel *standmodel = m_model->getStandFontModel();

    for (const QJsonObject &obj : standmodel->getFontList()) {
        if (obj["Id"].toString() == name) {
            m_standard->setValue(obj["Name"].toString());
            return;
        }
    }

    m_standard->setValue(standmodel->getFontName() + tr(" (Unsupported font)"));
}

void FontSettingsWidget::onMonoFontChanged(const QString &name)
{
    FontModel *monomodel = m_model->getMonoFontModel();

    for (const QJsonObject &obj : monomodel->getFontList()) {
        if (obj["Id"].toString() == name) {
            m_mono->setValue(obj["Name"].toString());
            return;
        }
    }

    m_mono->setValue(monomodel->getFontName() + tr(" (Unsupported font)"));
}
