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

#include "defcategorywidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/optionwidget.h"
#include "model/category.h"
#include "defappmodel.h"
#include "widgets/defcategoryaddwidget.h"
#include "widgets/settingshead.h"

using namespace dcc;
using namespace dcc::defapp;
using namespace dcc::widgets;

DefCategoryWidget::DefCategoryWidget(const QString &name, QWidget *parent)
    : TranslucentFrame(parent)
{
    m_centralLayout = new QVBoxLayout;
    m_userGroup = new SettingsGroup;
    m_addWidget  = new DefCategoryAddWidget(this);
    m_headWidget = new SettingsHead;
    m_addWidget->setAccessibleName(name);
    m_headWidget->setTitle(name);
    m_userGroup->insertItem(0, m_headWidget);
    m_userGroup->insertItem(1, m_addWidget);
    m_centralLayout->addWidget(m_userGroup);

    m_centralLayout->setMargin(0);

    setLayout(m_centralLayout);
    setObjectName("DefCategoryWidget");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_headWidget, &SettingsHead::editChanged, this, &DefCategoryWidget::slotEditMode);
    connect(m_addWidget, &DefCategoryAddWidget::requestFrameAutoHide, this, &DefCategoryWidget::requestFrameAutoHide);
    connect(m_addWidget, &DefCategoryAddWidget::requestCreateFile, this, &DefCategoryWidget::requestCreateFile);
}

void DefCategoryWidget::setCategory(Category *const category)
{
    m_category = category;
    connect(category, &Category::defaultChanged, this, &DefCategoryWidget::onDefaultAppSet);
    connect(category, &Category::addedUserItem, this, &DefCategoryWidget::addItem);
    connect(category, &Category::removedUserItem, this, &DefCategoryWidget::removeItem);
    connect(category, &Category::categoryNameChanged, this, &DefCategoryWidget::setCategoryName);

    AppsItemChanged(category->getappItem());

    onDefaultAppSet(category->getDefault());
    setCategoryName(category->getName());

    m_headWidget->setEditEnable(!m_userMap.isEmpty());
}

void DefCategoryWidget::setDefault()
{
    OptionWidget *item = qobject_cast<OptionWidget *>(sender());
    Q_EMIT requestSetDefaultApp(m_category->getName(), m_valueMap.value(item));
    onDefaultAppSet(m_valueMap.value(item));
}

void DefCategoryWidget::addItem(const App &item)
{
    if (item.isUser) m_userMap.append(item);

    OptionWidget *widget = new OptionWidget;
    widget->setItem(item);
    widget->setMime(m_category->getName());
    widget->setFixedHeight(36);
    m_userGroup->insertItem(1, widget);
    m_valueMap.insert(widget, item);

    widget->setChecked(item.Id ==  m_category->getDefault().Id);

    connect(widget, &OptionWidget::setDefault, this, &DefCategoryWidget::setDefault);
    connect(widget, &OptionWidget::removeItem, this, [=] (const App &app) {
        Q_EMIT requestDelUserApp(m_categoryName, app);
    });

    if(!m_userMap.empty()) {
        m_headWidget->setEditEnable(true);
    }
}

void DefCategoryWidget::removeItem(const App &item)
{
    OptionWidget *w = m_valueMap.key(item);
    m_userGroup->removeItem(w);
    m_userMap.removeOne(w->getItem());
    m_valueMap.remove(m_valueMap.key(item));

    if(m_userMap.empty()) {
        m_headWidget->setEditEnable(false);
    }

    w->deleteLater();
}

void DefCategoryWidget::onDefaultAppSet(const App &app)
{
    for (OptionWidget *item : m_valueMap.keys()) {
        item->setChecked(item->getItem().Id == app.Id);
    }
}

void DefCategoryWidget::slotEditMode(bool edit)
{
    if (m_userMap.size() != 0) {
        for (auto it = m_valueMap.constBegin(); it != m_valueMap.constEnd(); ++it) {
            it.key()->setDelete(edit);
        }
    }
}

void DefCategoryWidget::setCategoryName(const QString &name)
{
    m_categoryName = name;
    m_addWidget->setCategory(name);
}

void DefCategoryWidget::AppsItemChanged(const QList<App> &list)
{
    for (auto it = m_valueMap.constBegin(); it != m_valueMap.constEnd(); ++it) {
        m_userGroup->removeItem(it.key());
        it.key()->deleteLater();
    }

    m_valueMap.clear();

    for (const App& app : list) {
        addItem(app);
    }

    m_headWidget->setEditEnable(!m_userMap.isEmpty());

    onDefaultAppSet(m_category->getDefault());
}
