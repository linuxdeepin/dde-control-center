/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "bootwidget.h"
#include "window/modules/commoninfo/commonbackgrounditem.h"
#include "window/modules/commoninfo/commoninfomodel.h"

#include "window/utils.h"

#include "widgets/switchwidget.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/basiclistdelegate.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QListView>

using namespace dcc;
using namespace widgets;
using namespace DTK_WIDGET_NAMESPACE;
using namespace DCC_NAMESPACE;
using namespace commoninfo;

BootWidget::BootWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    SettingsGroup *groupBackground = new SettingsGroup;
    SettingsGroup *groupOther = new SettingsGroup;

    m_background = new CommonBackgroundItem();

    QVBoxLayout *listLayout = new QVBoxLayout;
    listLayout->setSpacing(0);
    listLayout->setMargin(0);

    m_bootList = new DListView();
    m_bootList->setAutoScroll(false);
    m_bootList->setFrameShape(QFrame::NoFrame);
    m_bootList->setDragDropMode(QListView::DragDrop);
    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->setDragEnabled(false);
    m_bootList->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    m_bootList->setSelectionMode(DListView::SingleSelection); // 单选

    m_updatingLabel = new TipsLabel(tr("Updating..."));
    m_updatingLabel->setVisible(false);

    listLayout->addSpacing(10);
    listLayout->addWidget(m_bootList, 0, Qt::AlignHCenter);
    listLayout->addStretch();
    listLayout->addWidget(m_updatingLabel, 0, Qt::AlignHCenter);
    listLayout->addSpacing(10);
    m_background->setLayout(listLayout);

    m_bootDelay = new SwitchWidget();
    //~ contents_path /commoninfo/Boot Menu
    m_bootDelay->setTitle(tr("Startup Delay"));

    m_theme = new SwitchWidget();
    //~ contents_path /commoninfo/Boot Menu
    m_theme->setTitle(tr("Theme"));

    TipsLabel *backgroundLabel = new TipsLabel(tr("Click the option in boot menu to set it as the first boot, and drag and drop a picture to change the background."));
    backgroundLabel->setWordWrap(true);
    backgroundLabel->setContentsMargins(16, 0, 10, 0);

    TipsLabel *themeLbl = new TipsLabel(tr("Switch theme on to view it in boot menu"));
    themeLbl->setWordWrap(true);
    themeLbl->setContentsMargins(16, 0, 10, 0);

    groupBackground->appendItem(m_background);
    groupOther->appendItem(m_bootDelay);
    groupOther->appendItem(m_theme);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(groupBackground);
    layout->addSpacing(8);
    layout->addWidget(backgroundLabel);
    layout->addSpacing(8);
    layout->addWidget(groupOther);
    layout->addWidget(themeLbl);
    layout->addStretch();

    setLayout(layout);
    //~ contents_path /commoninfo/Boot Menu
    setWindowTitle(tr("Boot Menu"));

    connect(m_theme, &SwitchWidget::checkedChanged, this, &BootWidget::enableTheme);
    connect(m_bootDelay, &SwitchWidget::checkedChanged, this, &BootWidget::bootdelay);
    connect(m_bootList, static_cast<void (DListView::*)(const QModelIndex &previous)>(&DListView::currentChanged),
            this, &BootWidget::onCurrentItem);
    connect(m_background, &CommonBackgroundItem::requestEnableTheme, this, &BootWidget::enableTheme);
    connect(m_background, &CommonBackgroundItem::requestSetBackground, this, &BootWidget::requestSetBackground);
}

void BootWidget::setDefaultEntry(const QString &value)
{
    m_defaultEntry = value;

    blockSignals(true);
    for (int i = 0; i < m_bootList->count(); i++) {
        QModelIndex itemIndex = m_bootList->model()->index(i, 0);
        QString itemText = itemIndex.data().toString();
        if (itemText == value) {
            m_bootList->setCurrentIndex(itemIndex);
            m_bootList->model()->setData(itemIndex, Qt::CheckState::Checked, Qt::CheckStateRole);
            break;
        }
    }
    blockSignals(false);
}

void BootWidget::setModel(CommonInfoModel *model)
{
    connect(model, &CommonInfoModel::bootDelayChanged, m_bootDelay, &SwitchWidget::setChecked);
    connect(model, &CommonInfoModel::themeEnabledChanged, m_theme, &SwitchWidget::setChecked);
    connect(model, &CommonInfoModel::defaultEntryChanged, this, &BootWidget::setDefaultEntry);
    connect(model, &CommonInfoModel::updatingChanged, m_updatingLabel, &SmallLabel::setVisible);
    connect(model, &CommonInfoModel::entryListsChanged, this, &BootWidget::setEntryList);
    connect(model, &CommonInfoModel::themeEnabledChanged, m_background, &CommonBackgroundItem::setThemeEnable);
    connect(model, &CommonInfoModel::backgroundChanged, m_background, &CommonBackgroundItem::updateBackground);

    m_bootDelay->setChecked(model->bootDelay());
    m_theme->setChecked(model->themeEnabled());
    m_updatingLabel->setVisible(model->updating());
    m_background->setThemeEnable(model->themeEnabled());

    setEntryList(model->entryLists());
    setDefaultEntry(model->defaultEntry());

    m_background->updateBackground(model->background());
}

void BootWidget::setEntryList(const QStringList &list)
{
    auto itemModel = new QStandardItemModel(this);
    m_bootList->setModel(itemModel);

    QModelIndex currentIndex;
    for (int i = 0; i < list.count(); i++) {
        const QString entry = list.at(i);

        DStandardItem *item = new DStandardItem();
        item->setText(entry);
        item->setCheckable(true);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);

        onCurrentItem(item->index());

        itemModel->appendRow(item);

        if (m_defaultEntry == entry) {
            currentIndex = item->index();
        }
    }
    m_bootList->setCurrentIndex(currentIndex);
}

void BootWidget::onCurrentItem(const QModelIndex &previousIdx)
{
    QString preText = previousIdx.data().toString();
    if (preText.isEmpty())
        return;

    // 获取当前被选项
    QModelIndex curIndex = m_bootList->currentIndex();
    QString curText = curIndex.data().toString();
    if (curText.isEmpty())
        return;
    if (curText != preText) {
        Q_EMIT defaultEntry(curText);
    }

    m_bootList->model()->setData(previousIdx, Qt::CheckState::Unchecked, Qt::CheckStateRole);
    m_bootList->model()->setData(curIndex, Qt::CheckState::Checked, Qt::CheckStateRole);
}
