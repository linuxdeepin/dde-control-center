// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "kblayoutsettingwidget.h"

#include "src/plugin-keyboard/operation/keyboardmodel.h"
#include "src/plugin-keyboard/window/keylabel.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"

#include <DStackedWidget>

#include <QComboBox>
#include <QDebug>
#include <QList>
#include <QSpinBox>
#include <QStringList>

using namespace DCC_NAMESPACE;

KBLayoutSettingWidget::KBLayoutSettingWidget(QWidget *parent)
    : QWidget(parent)
    , m_bEdit(false)
    , m_kbLayoutListView(new KBLayoutListView(this))
    , m_addLayoutAction(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *headLayout = new QHBoxLayout();

    TitleLabel *headTitle = new TitleLabel(tr("Keyboard Layout"));
    DFontSizeManager::instance()->bind(headTitle,
                                       DFontSizeManager::T5,
                                       QFont::DemiBold); // 设置label字体
    headLayout->addWidget(headTitle);
    headTitle->setContentsMargins(10, 0, 0, 0);

    m_editKBLayout = new DCommandLinkButton(tr("Edit"));
    m_editKBLayout->setObjectName("Edit");
    headLayout->addStretch();
    headLayout->addWidget(m_editKBLayout);
    mainLayout->addLayout(headLayout);

    m_kbLayoutModel = new QStandardItemModel(m_kbLayoutListView);
    m_kbLayoutModel->setObjectName("KbLayoutModel");
    m_kbLayoutListView->setAccessibleName("List_kblayoutlist");
    m_kbLayoutListView->setObjectName("KbLayoutListView");
    m_kbLayoutListView->setModel(m_kbLayoutModel);

    DStandardItem *footItem = new DStandardItem(tr("Add Keyboard Layout") + "...");
    footItem->setTextColorRole(DPalette::Highlight);
    m_kbLayoutModel->appendRow(footItem);

    QMargins itemMargins(m_kbLayoutListView->itemMargins());
    itemMargins.setLeft(10);
    m_kbLayoutListView->setItemMargins(itemMargins);
    m_kbLayoutListView->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_kbLayoutListView);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    connect(m_editKBLayout, &QPushButton::clicked, this, &KBLayoutSettingWidget::onEditClicked);
    connect(m_kbLayoutListView,
            &DListView::clicked,
            this,
            &KBLayoutSettingWidget::onKBLayoutChanged);
    connect(m_kbLayoutListView,
            &KBLayoutListView::currentChangedSignal,
            this,
            &KBLayoutSettingWidget::onKBCurrentChanged);
}

void KBLayoutSettingWidget::setModel(KeyboardModel *model)
{
    m_model = model;

    connect(model, &KeyboardModel::userLayoutChanged, this, &KBLayoutSettingWidget::onAddKeyboard);
    connect(model, &KeyboardModel::curLayoutChanged, this, &KBLayoutSettingWidget::onDefault);

    QMap<QString, QString> map = model->userLayout();

    for (auto i(map.begin()); i != map.end(); ++i) {
        onAddKeyboard(i.key(), i.value());
    }
}

void KBLayoutSettingWidget::onUpdateKBLayoutList()
{
    QMap<QString, QString> map = m_model->userLayout();

    for (auto i(map.begin()); i != map.end(); ++i) {
        onAddKeyboard(i.key(), i.value());
    }

    m_bEdit = true;
    onEditClicked();
}

void KBLayoutSettingWidget::onAddKeyboard(const QString &id, const QString &value)
{
    if (m_kbLangList.contains(id))
        return;
    DStandardItem *kbLayoutItem = new DStandardItem(value);
    kbLayoutItem->setData(id, KBLangIdRole);
    // 去除最后一个item
    DStandardItem *endItem = nullptr;
    if (m_kbLayoutModel->rowCount() > 0) {
        endItem = dynamic_cast<DStandardItem *>(
                m_kbLayoutModel->takeItem(m_kbLayoutModel->rowCount() - 1, 0));
        m_kbLayoutModel->removeRow(m_kbLayoutModel->rowCount() - 1);
    }

    // 按用户键盘布局列表顺序显示
    int index = 0;
    for (int i = m_kbLayoutModel->rowCount() - 1; i >= 0; --i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
        if (item == nullptr) {
            return;
        }
        if (m_model->getUserLayoutList().indexOf(id)
            > m_model->getUserLayoutList().indexOf(item->data(KBLangIdRole).toString())) {
            index = i + 1;
            break;
        }
    }
    m_kbLayoutModel->insertRow(index, kbLayoutItem);
    m_kbLangList << id;

    // 添加最后一个item
    if (endItem != nullptr) {
        m_kbLayoutModel->appendRow(endItem);
    }

    m_editKBLayout->setVisible(m_kbLangList.size() > 1);

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
        for (int i = 0; i < row_count - 1; ++i) {
            DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
            if (item && (item->checkState() == Qt::Unchecked)) {
                creatDelIconAction(item);
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
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
        if (item && (item->text() == value)) {
            item->setCheckState(Qt::Checked);
            if (m_bEdit) {
                item->setActionList(Qt::RightEdge, {});
            }
            // 滚动到当前选中项
            onKBCurrentChanged(m_kbLayoutModel->index(i, 0));
        } else {
            item->setCheckState(Qt::Unchecked);
            if (m_bEdit) {
                creatDelIconAction(item);
            }
        }
    }
}

void KBLayoutSettingWidget::creatDelIconAction(DStandardItem *item)
{
    DViewItemAction *iconAction =
            new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true);
    iconAction->setIcon(DStyle::standardIcon(style(), DStyle::SP_DeleteButton));
    item->setActionList(Qt::RightEdge, { iconAction });
    connect(iconAction, &DViewItemAction::triggered, this, [this, item] {
        m_kbLangList.removeOne(item->data(KBLangIdRole).toString());
        int idx = m_kbLayoutModel->indexFromItem(item).row();
        Q_EMIT delUserLayout(item->text());
        m_kbLayoutModel->removeRow(idx);
        m_kbLayoutListView->adjustSize();
        m_kbLayoutListView->update();
        m_editKBLayout->setVisible(m_kbLangList.size() > 1);
    });
}

void KBLayoutSettingWidget::onKBLayoutChanged(const QModelIndex &index)
{
    if (index.row() == m_kbLayoutListView->count() - 1) {
        onLayoutAdded();
        return;
    }
    if (m_bEdit) {
        return;
    }
    int row_count = m_kbLayoutModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_kbLayoutModel->item(i, 0);
        if (item && (index.row() == i)) {
            item->setCheckState(Qt::Checked);
            Q_EMIT requestCurLayoutAdded(item->text());
        } else if (item) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void KBLayoutSettingWidget::onKBCurrentChanged(const QModelIndex &current)
{
    if (current.row() == m_kbLayoutListView->count() - 1) {
        return;
    }
    QSize itemSize = m_kbLayoutListView->itemDelegate()->sizeHint(QStyleOptionViewItem(), current);
    int top = current.row() * (itemSize.height() + m_kbLayoutModel->span(current).height());

    QRect visibleRect = m_kbLayoutListView->visibleRegion().boundingRect();
    QRect itemRect(0, top, visibleRect.width(), itemSize.height());

    if (!visibleRect.contains(itemRect)) {
        if (visibleRect.bottom() < itemRect.bottom()) {
            //            m_contentWidget->scrollTo(itemRect.bottom() - visibleRect.bottom());
        } else {
            //            m_contentWidget->scrollTo(itemRect.top() - visibleRect.top());
        }
    }
}

void KBLayoutSettingWidget::onLayoutAdded()
{
    m_bEdit = false;

    m_editKBLayout->setText(tr("Edit"));
    int row_count = m_kbLayoutModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_kbLayoutModel->item(i, 0));
        if (item && (item->checkState() == Qt::Unchecked)) {
            item->setActionList(Qt::RightEdge, {});
        }
    }

    Q_EMIT layoutAdded(m_kbLangList);
}
