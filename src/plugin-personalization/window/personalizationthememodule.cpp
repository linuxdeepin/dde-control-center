/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "personalizationthememodule.h"
#include "model/thememodel.h"
#include "personalizationworker.h"

#include "widgets/widgetmodule.h"
#include "widgets/dcclistview.h"

#include <QScroller>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

const QMargins ScrollAreaOffsetMargins(0, 0, 10, 0);
#define IDRole (DTK_NAMESPACE::UserRole + 1)

PersonalizationThemeModule::PersonalizationThemeModule(ThemeModel *model, PersonalizationWorker *work, QObject *parent)
  : ModuleObject(parent)
  , m_itemModel(new QStandardItemModel(this))
  , m_model(model)
  , m_work(work)
{
    setChildType(ModuleObject::Page);
    appendChild(new WidgetModule<DCCListView>("perssonalizationThemeList", tr("Theme List"), this, &PersonalizationThemeModule::initThemeListView));
    appendChild(new WidgetModule<QWidget>());

    connect(m_model, &ThemeModel::defaultChanged, this, &PersonalizationThemeModule::setDefault);
    connect(m_model, &ThemeModel::itemAdded, this, &PersonalizationThemeModule::onAddItem);
    connect(m_model, &ThemeModel::picAdded, this, &PersonalizationThemeModule::onSetPic);
    connect(m_model, &ThemeModel::itemRemoved, this, &PersonalizationThemeModule::onRemoveItem);

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

void PersonalizationThemeModule::initThemeListView(DCCListView *listview)
{
    listview->setAccessibleName("List_perssonalizationthemelist");
    listview->setModel(m_itemModel);
    listview->setEditTriggers(QListView::NoEditTriggers);
    listview->setSelectionMode(QAbstractItemView::NoSelection);
    listview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    listview->setViewportMargins(ScrollAreaOffsetMargins);

    connect(listview, &DListView::clicked, this, &PersonalizationThemeModule::onClicked);

    QScroller *scroller = QScroller::scroller(listview->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);
}


void PersonalizationThemeModule::onAddItem(const QJsonObject &json)
{
    if (m_jsonMap.values().contains(json))
        return;

    const QString &title = json["Id"].toString();
    m_jsonMap.insert(title, json);

    DStandardItem *item = new DStandardItem;

    //translations
    if (json["type"] == "gtk") {
        if (title == "deepin") {
            item->setText(tr("Light"));
        } else if (title == "deepin-dark") {
            item->setText(tr("Dark"));
        } else if (title == "deepin-auto") {
            item->setText(tr("Auto"));
        } else {
            item->setText(title);
        }
    } else {
        item->setText(title == "deepin" ? QString("deepin (%1)").arg(tr("Default")) : title);
    }

    item->setData(title, IDRole); //set id data
    item->setCheckState(title == m_model->getDefault() ? Qt::Checked : Qt::Unchecked);
    m_itemModel->appendRow(item);
}

void PersonalizationThemeModule::setDefault(const QString &name)
{
    for (int i = 0; i < m_itemModel->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_itemModel->item(i, 0));
        item->setCheckState((item->data(IDRole).toString() == name) ? Qt::Checked : Qt::Unchecked);
    }
}

void PersonalizationThemeModule::onSetPic(const QString &id, const QString &picPath)
{
    for (int i = 0; i < m_itemModel->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_itemModel->item(i, 0));

        if (item->data(IDRole).toString() != id)
            continue;

        DViewItemActionList list;
        QPixmap pxmap = QPixmap(picPath);
        DViewItemAction *iconAction = new DViewItemAction(Qt::AlignLeft, pxmap.size() /*/ devicePixelRatioF()*/);
        iconAction->setIcon(QIcon(pxmap));
        list << iconAction;
        item->setActionList(Qt::BottomEdge, list);

        return;
    }
}

void PersonalizationThemeModule::onRemoveItem(const QString &id)
{
    for (int i = 0; i < m_itemModel->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_itemModel->item(i, 0));
        if (item->data(IDRole).toString() == id) {
            m_itemModel->removeRow(i);
            break;
        }
    }
}

void PersonalizationThemeModule::onClicked(const QModelIndex &index)
{
    if (m_jsonMap.contains(index.data(IDRole).toString()))
        m_work->setDefault(m_jsonMap.value(index.data(IDRole).toString()));
}
