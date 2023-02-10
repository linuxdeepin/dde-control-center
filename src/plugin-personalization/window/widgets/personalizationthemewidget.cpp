//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationthemewidget.h"
#include "themeitem.h"
#include "model/thememodel.h"

#include <QMap>
#include <QJsonObject>
#include <QBoxLayout>

#include <QLabel>

PersonalizationThemeWidget::PersonalizationThemeWidget(QWidget *parent)
    : QWidget(parent)
    , m_centerLayout(nullptr)
    , m_model(nullptr)
    , m_titleBelowPic(true)
{
    setAccessibleName("PersonalizationThemeWidget");
}

void PersonalizationThemeWidget::setModel(ThemeModel *const model)
{
    m_model = model;
    connect(m_model, &ThemeModel::defaultChanged, this, &PersonalizationThemeWidget::setDefault);
    connect(m_model, &ThemeModel::itemAdded, this, &PersonalizationThemeWidget::onAddItem);
    connect(m_model, &ThemeModel::picAdded, this, &PersonalizationThemeWidget::onSetPic);
    connect(m_model, &ThemeModel::itemRemoved, this, &PersonalizationThemeWidget::onRemoveItem);

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

void PersonalizationThemeWidget::onAddItem(const QJsonObject &json)
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
            //~ child_page General
            theme->setTitle(tr("Light"));
            theme->setAccessibleName("Light");
        } else if (title == "deepin-dark") {
            //~ contents_path /personalization/General
            //~ child_page General
            theme->setTitle(tr("Dark"));
            theme->setAccessibleName("Dark");
        } else if (title == "deepin-auto") {
            //~ contents_path /personalization/General
            //~ child_page General
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
    connect(theme, &ThemeItem::selectedChanged, this, &PersonalizationThemeWidget::onItemClicked);
}

void PersonalizationThemeWidget::setDefault(const QString &name)
{
    QMap<ThemeItem *, QJsonObject>::const_iterator it = m_valueMap.constBegin();
    while (it != m_valueMap.constEnd()) {
        it.key()->setSelected(it.key()->id().toString() == name);
        ++it;
    }
}

void PersonalizationThemeWidget::onItemClicked(const bool selected)
{
    if (selected) {
        ThemeItem *item = qobject_cast<ThemeItem *>(sender());
        Q_ASSERT(m_valueMap.contains(item));
        Q_EMIT requestSetDefault(m_valueMap[item]);
    }
}

void PersonalizationThemeWidget::onSetPic(const QString &id, const QString &picPath)
{
    QMap<ThemeItem *, QJsonObject>::const_iterator it = m_valueMap.constBegin();
    while (it != m_valueMap.constEnd()) {
        if (it.key()->id() == id) {
            it.key()->setPic(picPath);
            return;
        }
        ++it;
    }
}

void PersonalizationThemeWidget::onRemoveItem(const QString &id)
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

void PersonalizationThemeWidget::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    return;
}

void PersonalizationThemeWidget::setMainLayout(QBoxLayout *layout, bool titleBelowPic)
{
    m_centerLayout = layout;
    m_centerLayout->setMargin(0);
    m_centerLayout->setAlignment(Qt::AlignLeft);
    setLayout(m_centerLayout);
    m_titleBelowPic = titleBelowPic;
}
