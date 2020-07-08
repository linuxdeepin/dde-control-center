/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#include "kblayoutsettingwidget.h"
#include "window/utils.h"
#include "widgets/translucentframe.h"
#include "widgets/comboxwidget.h"
#include "modules/keyboard/keylabel.h"
#include "modules/keyboard/keyboardmodel.h"
#include "widgets/titlelabel.h"

#include <DFloatingButton>
#include <DAnchors>

#include <QStringList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>
#include <QList>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;
using namespace dcc::widgets;

KBLayoutSettingWidget::KBLayoutSettingWidget(QWidget *parent)
    : QWidget(parent)
    , m_bEdit(false)
    , m_kbLayoutListView(new MyListView())
    , m_switchLayoutListView(new DListView())
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(List_Interval);
    QHBoxLayout *headLayout = new QHBoxLayout();
    headLayout->setMargin(0);

    TitleLabel *headTitle = new TitleLabel(tr("Keyboard Layout"));
    headLayout->addWidget(headTitle);
    headTitle->setContentsMargins(15,0,0,0);
    headLayout->addStretch();

    m_editKBLayout = new DCommandLinkButton(tr("Edit"));
    headLayout->addWidget(m_editKBLayout);
    mainLayout->addLayout(headLayout);

    m_kbLayoutModel = new QStandardItemModel(m_kbLayoutListView);
    m_kbLayoutListView->setAccessibleName("List_kblayoutlist");
    m_kbLayoutListView->setModel(m_kbLayoutModel);
    m_kbLayoutListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_kbLayoutListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_kbLayoutListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_kbLayoutListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_kbLayoutListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_kbLayoutListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_kbLayoutListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_kbLayoutListView->setContentsMargins(ListViweItemMargin);
    mainLayout->addWidget(m_kbLayoutListView);
    mainLayout->addSpacing(List_Interval);
    m_switchTitle = new TitleLabel(tr("Switch Layouts (Multiple)"));
    mainLayout->addWidget(m_switchTitle);
    mainLayout->addWidget(m_switchLayoutListView);
    mainLayout->setContentsMargins(0, 0, 15, 0);

    QMap<int, QString> shortCutMap;
    shortCutMap.insert(1, QString("Ctrl+Shift"));
    shortCutMap.insert(2, QString("Alt+Shift"));
    shortCutMap.insert(4, QString("Super+Space"));

    m_switchLayoutModel = new QStandardItemModel(m_switchLayoutListView);
    QMap<int, QString>::iterator iter;
    for (iter = shortCutMap.begin(); iter != shortCutMap.end(); ++iter) {
        DStandardItem *shortCutItem = new DStandardItem(iter.value());
        shortCutItem->setData(iter.key(), SwitchValueRole);
        m_switchLayoutModel->appendRow(shortCutItem);
    }
    m_switchLayoutListView->setAccessibleName("List_kbswitchlayoutlist");
    m_switchLayoutListView->setModel(m_switchLayoutModel);
    m_switchLayoutListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_switchLayoutListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_switchLayoutListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_switchLayoutListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_switchLayoutListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_switchLayoutListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_switchLayoutListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_switchLayoutListView->setContentsMargins(ListViweItemMargin);

    m_comboWidget = new ComboxWidget;
    //~ contents_path /keyboard/Keyboard Layout
    m_comboWidget->setTitle(tr("Applies to"));
    QStringList comboxOptions;
    comboxOptions << tr("System") << tr("Application");
    m_comboWidget->setComboxOption(comboxOptions);
    mainLayout->addWidget(m_comboWidget);
    mainLayout->addStretch();

    QWidget *widget = new QWidget(this);
    widget->setLayout(mainLayout);
    m_contentWidget = new ContentWidget(this);
    m_contentWidget->setContent(widget);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setMargin(0);//
    vLayout->addWidget(m_contentWidget);

    DFloatingButton *addLayout = new DFloatingButton(DStyle::SP_IncreaseElement, this);
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);//
    btnLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    btnLayout->addWidget(addLayout);
    vLayout->addLayout(btnLayout);
    setLayout(vLayout);

    connect(addLayout, &DFloatingButton::clicked, this, &KBLayoutSettingWidget::onLayoutAdded);
    connect(m_editKBLayout, &QPushButton::clicked, this, &KBLayoutSettingWidget::onEditClicked);
    connect(m_comboWidget, &ComboxWidget::onIndexChanged, this, &KBLayoutSettingWidget::onSwitchKBLayoutScope);
    connect(m_kbLayoutListView, &DListView::clicked, this, &KBLayoutSettingWidget::onKBLayoutChanged);
    connect(m_switchLayoutListView, &DListView::clicked, this, &KBLayoutSettingWidget::onSwitchKBChanged);
    connect(m_kbLayoutListView, &MyListView::currentChangedSignal, this, &KBLayoutSettingWidget::onKBCurrentChanged);
}

void KBLayoutSettingWidget::setModel(KeyboardModel *model)
{
    m_model = model;

    connect(model, &KeyboardModel::userLayoutChanged, this, &KBLayoutSettingWidget::onAddKeyboard);
    connect(model, &KeyboardModel::curLayoutChanged, this, &KBLayoutSettingWidget::onDefault);
    connect(model, &KeyboardModel::kbSwitchChanged, this, &KBLayoutSettingWidget::onSwitchKB);
    connect(model, &KeyboardModel::layoutScopeChanged, this, &KBLayoutSettingWidget::onLayoutScope);

    QMap<QString, QString> map = model->userLayout();

    for (auto i(map.begin()); i != map.end(); ++i) {
        onAddKeyboard(i.key(), i.value());
    }

    onSwitchKB(model->kbSwitch());
    onLayoutScope(model->layoutScope());
}

void KBLayoutSettingWidget::onAddKeyboard(const QString &id, const QString &value)
{
    if (m_kbLangList.contains(id))
        return;
    DStandardItem *kbLayoutItem = new DStandardItem(value);
    kbLayoutItem->setData(id, KBLangIdRole);
    // 按用户键盘布局列表顺序显示
    int index = 0;
    for (int i = m_kbLayoutModel->rowCount() - 1; i >= 0; --i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
        if (item == nullptr) {
            return;
        }
        if (m_model->getUserLayoutList().indexOf(id) > m_model->getUserLayoutList().indexOf(item->data(KBLangIdRole).toString())) {
            index = i + 1;
            break;
        }
    }
    m_kbLayoutModel->insertRow(index, kbLayoutItem);
    m_kbLangList << id;
    setUIVisible();
    onDefault(m_model->curLayout());
    m_kbLayoutListView->adjustSize();
    m_kbLayoutListView->update();
}

void KBLayoutSettingWidget::onEditClicked()
{
    if (m_kbLangList.count() < 2) {
        return;
    }
    m_bEdit = !m_bEdit;
    if (m_bEdit) {
        m_editKBLayout->setText(tr("Done"));
        int row_count = m_kbLayoutModel->rowCount();
        for (int i = 0; i < row_count; ++i) {
            DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
            if (item && (item->checkState() == Qt::Unchecked)) {
                DViewItemAction *iconAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true);
                iconAction->setIcon(DStyle::standardIcon(style(), DStyle::SP_DeleteButton));
                item->setActionList(Qt::RightEdge, {iconAction});
                connect(iconAction, &DViewItemAction::triggered, this, [this, item] {
                    m_kbLangList.removeOne(item->data(KBLangIdRole).toString());
                    int idx = m_kbLayoutModel->indexFromItem(item).row();
                    Q_EMIT delUserLayout(item->text());
                    m_kbLayoutModel->removeRow(idx);
                    m_kbLayoutListView->adjustSize();
                    m_kbLayoutListView->update();
                    setUIVisible();
                });
            }
        }
    } else {
        m_editKBLayout->setText(tr("Edit"));
        int row_count = m_kbLayoutModel->rowCount();
        for (int i = 0; i < row_count; ++i) {
            DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
            if (item && (item->checkState() == Qt::Unchecked)) {
                item->setActionList(Qt::RightEdge, {});
            }
        }
    }
}

void KBLayoutSettingWidget::onDefault(const QString &value)
{
    int row_count = m_kbLayoutModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_kbLayoutModel->item(i, 0);
        if (item && (item->text() == value)) {
            item->setCheckState(Qt::Checked);
            // 滚动到当前选中项
            onKBCurrentChanged(m_kbLayoutModel->index(i, 0));
        } else {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void KBLayoutSettingWidget::onKBLayoutChanged(const QModelIndex &index)
{
    if (m_bEdit) {
        return;
    }
    int row_count = m_kbLayoutModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_kbLayoutModel->item(i, 0);
        if (item && (index.row() == i)) {
            item->setCheckState(Qt::Checked);
            Q_EMIT requestCurLayoutAdded(item->text());
        } else if(item) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void KBLayoutSettingWidget::onKBCurrentChanged(const QModelIndex &current)
{
    QSize itemSize = m_kbLayoutListView->itemDelegate()->sizeHint(QStyleOptionViewItem(), current);
    int top = current.row() * (itemSize.height() + m_kbLayoutModel->span(current).height());

    QRect visibleRect = m_kbLayoutListView->visibleRegion().boundingRect();
    QRect itemRect(0, top, visibleRect.width(), itemSize.height());

    if (!visibleRect.contains(itemRect)) {
        if (visibleRect.bottom() < itemRect.bottom()) {
            m_contentWidget->scrollTo(itemRect.bottom() - visibleRect.bottom());
        } else {
            m_contentWidget->scrollTo(itemRect.top() - visibleRect.top());
        }
    }
}

void KBLayoutSettingWidget::onSwitchKBChanged(const QModelIndex &index)
{
    QStandardItem *item = m_switchLayoutModel->item(index.row(), index.column());
    Qt::CheckState state = item->checkState();
    if (state == Qt::Checked) {
        item->setCheckState(Qt::Unchecked);
    } else {
        item->setCheckState(Qt::Checked);
    }
    int median = 0;
    int row_count = m_switchLayoutModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *items = m_switchLayoutModel->item(i, 0);
        if (items && (items->checkState() == Qt::Checked)) {
            median = items->data(SwitchValueRole).toInt() | median;
        }
    }

    Q_EMIT requestSwitchKBLayout(median);
}

void KBLayoutSettingWidget::onSwitchKB(int kbSwitch)
{
    int row_count = m_switchLayoutModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_switchLayoutModel->item(i, 0);
        if (item && ((kbSwitch & item->data(SwitchValueRole).toInt()) != 0)) {
            item->setCheckState(Qt::Checked);
        }
    }
}

void KBLayoutSettingWidget::onLayoutScope(const int value)
{
    m_comboWidget->comboBox()->setCurrentIndex(value);
}

void KBLayoutSettingWidget::onLayoutAdded()
{
    Q_EMIT layoutAdded(m_kbLangList);
}

void KBLayoutSettingWidget::setUIVisible()
{
    bool uiVisible = true;
    if (m_kbLangList.size() > 1) {
        uiVisible = true;
    } else {
        uiVisible = false;
    }
    m_editKBLayout->setVisible(uiVisible);
    m_switchTitle->setVisible(uiVisible);
    m_switchLayoutListView->setVisible(uiVisible);
    m_comboWidget->setVisible(uiVisible);
}
