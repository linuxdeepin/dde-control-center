/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "perssonalizationthemewidget.h"
#include "themeitem.h"
#include "modules/personalization/model/thememodel.h"

#include <DApplicationHelper>

#include <QMap>
#include <QJsonObject>
#include <QBoxLayout>

#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

const int minMargain = 20;
const int maxSpacing = 40;

PerssonalizationThemeWidget::PerssonalizationThemeWidget(SettingsItem *parent)
    : SettingsItem(parent)
    , m_centerLayout(nullptr)
    , m_model(nullptr)
    , m_titleBelowPic(true)
    , m_personalWidth(0)
{
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        addBackground();
    }
}

void PerssonalizationThemeWidget::setModel(dcc::personalization::ThemeModel *const model)
{
    m_model = model;
    connect(m_model, &dcc::personalization::ThemeModel::defaultChanged, this, &PerssonalizationThemeWidget::setDefault);
    connect(m_model, &dcc::personalization::ThemeModel::itemAdded, this, &PerssonalizationThemeWidget::onAddItem);
    connect(m_model, &dcc::personalization::ThemeModel::picAdded, this, &PerssonalizationThemeWidget::onSetPic);
    connect(m_model, &dcc::personalization::ThemeModel::itemRemoved, this, &PerssonalizationThemeWidget::onRemoveItem);

    QMap<QString, QJsonObject> itemList = m_model->getList();

    for (auto it(itemList.constBegin()); it != itemList.constEnd(); ++it) {
        onAddItem(it.value());
    }

    setDefault(m_model->getDefault());

    QMap<QString, QString> picList = m_model->getPicList();

    for (auto it(picList.constBegin()); it != picList.constEnd(); ++it) {
        onSetPic(it.key(), it.value());
    }
}

void PerssonalizationThemeWidget::onAddItem(const QJsonObject &json)
{
    if (m_valueMap.values().contains(json))
        return;

    ThemeItem *theme = new ThemeItem(m_titleBelowPic, this);
    const QString &title = json["Id"].toString();
    theme->setId(title);

    //translations
    if (json["type"] == "gtk") {
        if (title == "deepin") {
            //~ contents_path /personalization/General
            theme->setTitle(tr("Light"));
            theme->setAccessibleName("Light");
        } else if (title == "deepin-dark") {
            //~ contents_path /personalization/General
            theme->setTitle(tr("Dark"));
            theme->setAccessibleName("Dark");
        } else if (title == "deepin-auto") {
            //~ contents_path /personalization/General
            theme->setTitle(tr("Auto"));
            theme->setAccessibleName("Auto");
        } else {
            theme->setTitle(title);
            theme->setAccessibleName(title);

        }
    } else {
        theme->setTitle(title == "deepin" ? QString("deepin (%1)").arg(tr("Default")) : title);
        theme->setAccessibleName(title == "deepin" ? QString("deepin (%1)").arg(tr("Default")) : title);
    }

    theme->setSelected(title == m_model->getDefault());

    m_valueMap.insert(theme, json);
    m_centerLayout->addWidget(theme);
    connect(theme, &ThemeItem::selectedChanged, this, &PerssonalizationThemeWidget::onItemClicked);
}

void PerssonalizationThemeWidget::setDefault(const QString &name)
{
    QMap<ThemeItem *, QJsonObject>::const_iterator it = m_valueMap.constBegin();
    while (it != m_valueMap.constEnd()) {
        it.key()->setSelected(it.key()->id().toString() == name);
        ++it;
    }
}

void PerssonalizationThemeWidget::onItemClicked(const bool selected)
{
    if (selected) {
        ThemeItem *item = qobject_cast<ThemeItem *>(sender());
        Q_ASSERT(m_valueMap.contains(item));
        Q_EMIT requestSetDefault(m_valueMap[item]);
    }
}

void PerssonalizationThemeWidget::onSetPic(const QString &id, const QString &picPath)
{
    QMap<ThemeItem *, QJsonObject>::const_iterator it = m_valueMap.constBegin();
    while (it != m_valueMap.constEnd()) {
        if (it.key()->id() == id) {
            it.key()->setPic(picPath);
            if (DGuiApplicationHelper::isTabletEnvironment()) {
              updateMargains(it.key());
            }
            return;
        }
        ++it;
    }
}

void PerssonalizationThemeWidget::onRemoveItem(const QString &id)
{
    QMap<ThemeItem *, QJsonObject>::iterator it = m_valueMap.begin();
    while (it != m_valueMap.end()) {
        if (it.key()->id() == id){
            delete it.key();
            m_valueMap.erase(it);
            return;
        }
        ++it;
    }
}

void PerssonalizationThemeWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    return;
}

void PerssonalizationThemeWidget::setMainLayout(QBoxLayout *layout, bool titleBelowPic)
{
    m_centerLayout = layout;
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        m_centerLayout->setSpacing(40);
        m_centerLayout->setContentsMargins(80, 42, 80, 13);
    } else {
        m_centerLayout->setMargin(0);
    }

    m_centerLayout->setAlignment(Qt::AlignLeft);
    setLayout(m_centerLayout);
    m_titleBelowPic = titleBelowPic;
}

void PerssonalizationThemeWidget::updateMargains(ThemeItem *item)
{
    // 左右边距 = 二级界面宽度　- 预留边距 - 总的主题图片宽度 - 总的空格
    int totalPicSize = item->themeSize().width() * m_valueMap.size();
    int totalSpacing = maxSpacing * (m_valueMap.size() - 1);
    int margain = (m_personalWidth - minMargain - totalPicSize - totalSpacing) / 2;
    if (margain < minMargain) {
        int space = maxSpacing - (minMargain - margain);
        m_centerLayout->setContentsMargins(minMargain, 42, minMargain, 13);
        m_centerLayout->setSpacing(space);
    } else {
        m_centerLayout->setContentsMargins(margain, 42, margain, 13);
    }
}
