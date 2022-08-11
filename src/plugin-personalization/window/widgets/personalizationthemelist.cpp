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
#include "model/thememodel.h"
#include "titlelabel.h"
//#include "window/utils.h"

#include <DListView>

#include <QVBoxLayout>
#include <QScroller>
#include <QPushButton>
#include <DSuggestButton>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

PersonalizationThemeList::PersonalizationThemeList(const QString &title, QWidget *parent)
    : QDialog(parent)
    , m_listview(new DListView)
{
    setAccessibleName("PersonalizationThemeList");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    TitleLabel *titleLabel = new TitleLabel(title);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    QStandardItemModel *model = new QStandardItemModel(this);
    m_listview->setAccessibleName("List_PersonalizationThemeList");
    m_listview->setModel(model);
    m_listview->setEditTriggers(QListView::NoEditTriggers);
    m_listview->setSelectionMode(QAbstractItemView::NoSelection);
    m_listview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listview->setViewportMargins(0, 0, 10, 0);

    layout->addWidget(m_listview);
    // 右侧偏移10像素给滚动条
    QMargins pageMargins(10, 10, 0, 10);
    layout->setContentsMargins(pageMargins);
    this->setLayout(layout);
    connect(m_listview, &DListView::clicked, this, &PersonalizationThemeList::onClicked);

    QScroller *scroller = QScroller::scroller(m_listview->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->addWidget(cancleBtn);
    cansaveLayout->addWidget(saveBtn);
    layout->addLayout(cansaveLayout);

    connect(cancleBtn, &QPushButton::clicked, this, &PersonalizationThemeList::reject);
    connect(saveBtn, &DSuggestButton::clicked, this, &PersonalizationThemeList::clickSaveBtn);
    resize(540, 640);
}

PersonalizationThemeList::~PersonalizationThemeList()
{
    QScroller *scroller = QScroller::scroller(m_listview->viewport());
    if (scroller) {
        scroller->stop();
    }
}

void PersonalizationThemeList::setModel(ThemeModel *const model)
{
    m_model = model;
    connect(m_model, &ThemeModel::defaultChanged, this, &PersonalizationThemeList::setDefault);
    connect(m_model, &ThemeModel::itemAdded, this, &PersonalizationThemeList::onAddItem);
    connect(m_model, &ThemeModel::picAdded, this, &PersonalizationThemeList::onSetPic);
    connect(m_model, &ThemeModel::itemRemoved, this, &PersonalizationThemeList::onRemoveItem);

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

void PersonalizationThemeList::onAddItem(const QJsonObject &json)
{
    if (m_jsonMap.values().contains(json))
        return;

    const QString &title = json["Id"].toString();
    m_jsonMap.insert(title, json);

    DStandardItem *item = new DStandardItem;

    // translations
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

    item->setData(title, IDRole); // set id data
    item->setCheckState(title == m_model->getDefault() ? Qt::Checked : Qt::Unchecked);
    qobject_cast<QStandardItemModel *>(m_listview->model())->appendRow(item);
}

void PersonalizationThemeList::setDefault(const QString &name)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(m_listview->model());

    for (int i = 0; i < model->rowCount(); ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(model->item(i, 0));
        item->setCheckState((item->data(IDRole).toString() == name) ? Qt::Checked : Qt::Unchecked);
    }
}

void PersonalizationThemeList::onSetPic(const QString &id, const QString &picPath)
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

void PersonalizationThemeList::onRemoveItem(const QString &id)
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

void PersonalizationThemeList::onClicked(const QModelIndex &index)
{
    setDefault(index.data(IDRole).toString());
}

void PersonalizationThemeList::clickSaveBtn()
{
    const QModelIndex &index = m_listview->currentIndex();
    if (m_jsonMap.contains(index.data(IDRole).toString()))
        Q_EMIT requestSetDefault(m_jsonMap.value(index.data(IDRole).toString()));

    accept();
}
