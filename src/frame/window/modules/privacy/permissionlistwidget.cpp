// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "window/modules/privacy/permissionlistwidget.h"
#include "widgets/multiselectlistview.h"

#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QMargins>

Q_DECLARE_METATYPE(QMargins)
PermissionListWidget::PermissionListWidget(const QList<PrivacyPage> &privactPages, QWidget *parent)
    : QWidget(parent)
    , m_lvnmpages(new dcc::widgets::MultiSelectListView(this))
{
    QStandardItemModel *modelpages = new QStandardItemModel(this);
    m_lvnmpages->setAccessibleName("permissionListView");
    m_lvnmpages->setFrameShape(QFrame::NoFrame);
    m_lvnmpages->setModel(modelpages);
    m_lvnmpages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvnmpages->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_lvnmpages->setViewportMargins(QMargins(10, 10, 10, 10));
    m_lvnmpages->setIconSize(QSize(32, 32));

    QVBoxLayout *centralLayout = new QVBoxLayout();
    centralLayout->setMargin(0);
    setLayout(centralLayout);

    for (auto &&page : privactPages) {
        DStandardItem *item = new DStandardItem(page.displayName);
        item->setData(page.mainPremission, Dtk::UserRole);
        item->setIcon(page.icon);
        item->setData(QVariant::fromValue(QMargins(10, 8, 10, 8)), Dtk::MarginsRole);
        modelpages->appendRow(item);
    }

    centralLayout->addWidget(m_lvnmpages);
    connect(m_lvnmpages->selectionModel(), &QItemSelectionModel::currentChanged, this, &PermissionListWidget::onCurrentChanged);
}

void PermissionListWidget::onCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous)
    if (current.isValid()) {
        Q_EMIT requestShowPage(current.data(Dtk::UserRole).toInt());
    }
}

void PermissionListWidget::setCurrent(int id)
{
    QAbstractItemModel *modelpages = m_lvnmpages->model();
    for (int i = 0; i < modelpages->rowCount(); i++) {
        if (modelpages->index(i, 0).data(Dtk::UserRole).toInt() == id) {
            m_lvnmpages->setCurrentIndex(modelpages->index(i, 0));
            break;
        }
    }
}
