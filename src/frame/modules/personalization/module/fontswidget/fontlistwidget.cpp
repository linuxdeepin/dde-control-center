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

#include "fontlistwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/optionitem.h"
#include "../../widget/fontitem.h"
#include "../../personalizationmodel.h"
#include "../../model/fontmodel.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

FontListWidget::FontListWidget(const QString &title, QWidget *parent)
    :ContentWidget(parent)
{
    m_mainLayout = new QVBoxLayout;
    m_mainGroup  = new SettingsGroup;
    m_widget     = new TranslucentFrame;

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    m_widget->setLayout(m_mainLayout);
    setTitle(title);
    setContent(m_widget);
}

void FontListWidget::setModel(FontModel *const model)
{
    m_model = model;
    connect(model, &FontModel::listChanged, this, &FontListWidget::setList);
    connect(model, &FontModel::defaultFontChanged, this, &FontListWidget::setDefault);

    setList(model->getFontList());
}

void FontListWidget::setList(const QList<QJsonObject> &list)
{
    for (auto it = m_valueMap.begin(); it != m_valueMap.end();) {
        m_mainGroup->removeItem(it.key());
        it.key()->deleteLater();
        it = m_valueMap.erase(it);
    }

    bool exist = false;
    for (QJsonObject item : list) {
        OptionItem *w = new OptionItem;
        w->setContentsMargins(10, 0, 10, 0);
        w->setAccessibleName("FontListWidget");
        FontItem   *font = new FontItem(item["Name"].toString());
        w->setTitleWidget(font);
        m_mainGroup->appendItem(w);
        m_valueMap.insert(w, item);
        connect(w, &OptionItem::selectedChanged, this, &FontListWidget::onItemClicked);

        if (!exist && item["Id"].toString() == m_model->getFontName())
            exist = true;
    }

    if (!exist)
    {
        OptionItem *w = new OptionItem;
        w->setContentsMargins(10, 0, 10, 0);

        FontItem   *font = new FontItem(m_model->getFontName() + tr(" (Unsupported font)"));
        w->setTitleWidget(font);

        m_mainGroup->appendItem(w);
        QJsonObject json;
        json.insert("Id", m_model->getFontName());
        m_valueMap.insert(w, json);

        w->setSelected(true);
        connect(w, &OptionItem::selectedChanged, this, [=] {
            w->setSelected(false);
        });
    }

    setDefault(m_model->getFontName());
}

void FontListWidget::setDefault(const QString &name)
{
    for (auto i(m_valueMap.cbegin()); i != m_valueMap.cend(); ++i)
        i.key()->setSelected(i.value()["Id"].toString() == name);
}

void FontListWidget::onItemClicked()
{
    OptionItem *item = qobject_cast<OptionItem *>(sender());
    Q_ASSERT(m_valueMap.contains(item));

    Q_EMIT requestSetDefault(m_valueMap[item]);
}
