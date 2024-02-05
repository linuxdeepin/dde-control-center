//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationthemelist.h"
#include "buttontuple.h"
#include "model/thememodel.h"
#include "titlelabel.h"

#include <DListView>

#include <QVBoxLayout>
#include <QScroller>
#include <QPushButton>
#include <DSuggestButton>
#include <DTitlebar>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

PersonalizationThemeList::PersonalizationThemeList(const QString &title, QWidget *parent)
    : DAbstractDialog(false, parent)
    , m_listview(new DListView)
{
    setAccessibleName("PersonalizationThemeList");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    DTitlebar *titleIcon = new DTitlebar();
    titleIcon->setFrameStyle(QFrame::NoFrame);//无边框
    titleIcon->setBackgroundTransparent(true);//透明
    titleIcon->setMenuVisible(false);
    titleIcon->setIcon(qApp->windowIcon());
    titleIcon->setTitle(title);
    layout->addWidget(titleIcon);

    QStandardItemModel *model = new QStandardItemModel(this);
    m_listview->setAccessibleName("List_PersonalizationThemeList");
    m_listview->setModel(model);
    m_listview->setEditTriggers(QListView::NoEditTriggers);
    m_listview->setSelectionMode(QAbstractItemView::NoSelection);
    m_listview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listview->setViewportMargins(0, 0, 10, 0);

    auto *viewLayout = new QVBoxLayout;
    viewLayout->setContentsMargins(10, 10, 0, 0);
    viewLayout->addWidget(m_listview);

    layout->addLayout(viewLayout);
    // 右侧偏移10像素给滚动条
    this->setLayout(layout);
    connect(m_listview, &DListView::clicked, this, &PersonalizationThemeList::onClicked);

    QScroller *scroller = QScroller::scroller(m_listview->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    auto *buttonTuple = new ButtonTuple(dccV23::ButtonTuple::Save, this);
    buttonTuple->setContentsMargins(10, 0, 10, 10);
    layout->addWidget(buttonTuple);

    QPushButton *cancelBtn = buttonTuple->leftButton();
    cancelBtn->setText(tr("Cancel"));
    QPushButton *saveBtn = buttonTuple->rightButton();
    saveBtn->setText(tr("Save"));
    connect(cancelBtn, &QPushButton::clicked, this, &PersonalizationThemeList::reject);
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

    for (auto &&key : m_model->keys()) {
        onAddItem(itemList.value(key));
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

    const QString &id = json["Id"].toString();
    const QString &name = json["Name"].toString();

    m_jsonMap.insert(id, json);

    DStandardItem *item = new DStandardItem;

    // translations
    if (json["type"] == "gtk") {
        if (id == "deepin") {
            item->setText(tr("Light"));
        } else if (id == "deepin-dark") {
            item->setText(tr("Dark"));
        } else if (id == "deepin-auto") {
            item->setText(tr("Auto"));
        } else {
            item->setText(id);
        }
    } else if (json["type"] == "icon") {
        // icon use "name" as title
        item->setText(id == "deepin" ? QString("deepin (%1)").arg(tr("Default")) : name);
    } else {
        // cursor use "id" as title
        item->setText(id == "deepin" ? QString("deepin (%1)").arg(tr("Default")) : id);
    }

    item->setData(id, IDRole); // set id data
    item->setCheckState(id == m_model->getDefault() ? Qt::Checked : Qt::Unchecked);
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
