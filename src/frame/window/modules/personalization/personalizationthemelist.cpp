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
#include "personalizationthemelist.h"
#include "modules/personalization/model/thememodel.h"
#include "window/utils.h"

#include <DListView>

#include <QVBoxLayout>
#include <QScroller>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

DWIDGET_USE_NAMESPACE

PerssonalizationThemeList::PerssonalizationThemeList(QWidget *parent)
    : QWidget(parent)
    , m_listview(new DListView)
{
    setAccessibleName("PerssonalizationThemeList");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    QStandardItemModel *model = new QStandardItemModel(this);
    m_listview->setAccessibleName("List_perssonalizationthemelist");
    m_listview->setModel(model);
    m_listview->setEditTriggers(QListView::NoEditTriggers);
    m_listview->setSelectionMode(QAbstractItemView::NoSelection);
    m_listview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listview->setViewportMargins(ScrollAreaOffsetMargins);

    layout->addWidget(m_listview);
    // 右侧偏移10像素给滚动条
    QMargins pageMargins(ThirdPageContentsMargins.left(), ThirdPageContentsMargins.top()
                         , ThirdPageContentsMargins.right() - 10, ThirdPageContentsMargins.bottom());
    layout->setContentsMargins(pageMargins);
    this->setLayout(layout);
    connect(m_listview, &DListView::clicked, this, &PerssonalizationThemeList::onClicked);

    QScroller *scroller = QScroller::scroller(m_listview->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);
}

PerssonalizationThemeList::~PerssonalizationThemeList()
{
    QScroller *scroller = QScroller::scroller(m_listview->viewport());
    if (scroller) {
        scroller->stop();
    }
}

void PerssonalizationThemeList::setModel(dcc::personalization::ThemeModel *const model)
{
    m_model = model;
    connect(m_model, &dcc::personalization::ThemeModel::defaultChanged, this, &PerssonalizationThemeList::setDefault);
    connect(m_model, &dcc::personalization::ThemeModel::itemAdded, this, &PerssonalizationThemeList::onAddItem);
    connect(m_model, &dcc::personalization::ThemeModel::picAdded, this, &PerssonalizationThemeList::onSetPic);
    connect(m_model, &dcc::personalization::ThemeModel::itemRemoved, this, &PerssonalizationThemeList::onRemoveItem);

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

void PerssonalizationThemeList::onAddItem(const QJsonObject &json)
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
    qobject_cast<QStandardItemModel *>(m_listview->model())->appendRow(item);
}

void PerssonalizationThemeList::setDefault(const QString &name)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(m_listview->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(model->item(i, 0));
        item->setCheckState((item->data(IDRole).toString() == name) ? Qt::Checked : Qt::Unchecked);
    }
}

void PerssonalizationThemeList::onSetPic(const QString &id, const QString &picPath)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(m_listview->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(model->item(i, 0));

        if (item->data(IDRole).toString() != id)
            continue;

        DViewItemActionList list;
        QPixmap pxmap = QPixmap(picPath);
        DViewItemAction *iconAction = new DViewItemAction(Qt::AlignLeft, pxmap.size() / devicePixelRatioF());
        iconAction->setIcon(QIcon(pxmap));
        list << iconAction;
        item->setActionList(Qt::BottomEdge, list);

        return;
    }
}

void PerssonalizationThemeList::onRemoveItem(const QString &id)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(m_listview->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(model->item(i, 0));
        if (item->data(IDRole).toString() == id) {
            model->removeRow(i);
            break;
        }
    }
}

void PerssonalizationThemeList::onClicked(const QModelIndex &index)
{
    if (m_jsonMap.contains(index.data(IDRole).toString()))
        Q_EMIT requestSetDefault(m_jsonMap.value(index.data(IDRole).toString()));
}
