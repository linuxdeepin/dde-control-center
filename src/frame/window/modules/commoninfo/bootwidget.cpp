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

#include <DTipLabel>
#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QScrollBar>
#include <QListView>

using namespace dcc;
using namespace widgets;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

BootWidget::BootWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    SettingsGroup *groupOther = new SettingsGroup;

    m_background = new CommonBackgroundItem();

    m_listLayout = new QVBoxLayout;
    m_listLayout->setSpacing(0);
    m_listLayout->setMargin(0);

    m_bootList = new DListView();
    m_bootList->setAutoScroll(false);
    m_bootList->setFrameShape(QFrame::NoFrame);
    m_bootList->setDragDropMode(QListView::DragDrop);
    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->setAutoFillBackground(false);
    m_bootList->setDragEnabled(false);
    m_bootList->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    m_bootList->setSelectionMode(DListView::NoSelection);
    m_bootList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_bootList->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_bootList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_bootList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    DPalette dp = DApplicationHelper::instance()->palette(m_bootList);
    dp.setColor(DPalette::Text, QColor(255, 255, 255));
    DApplicationHelper::instance()->setPalette(m_bootList, dp);

    m_updatingLabel = new TipsLabel(tr("Updating..."));
    m_updatingLabel->setVisible(false);

    m_listLayout->addSpacing(List_Interval);
    m_listLayout->addWidget(m_bootList);
    m_listLayout->addStretch();
    m_listLayout->addWidget(m_updatingLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    m_listLayout->addSpacing(List_Interval);
    m_background->setLayout(m_listLayout);

    m_bootDelay = new SwitchWidget();
    //~ contents_path /commoninfo/Boot Menu
    m_bootDelay->setTitle(tr("Startup Delay"));
#ifndef DCC_DISABLE_GRUB_THEME
    m_theme = new SwitchWidget();
    //~ contents_path /commoninfo/Boot Menu
    m_theme->setTitle(tr("Theme"));
#endif
    DTipLabel *backgroundLabel = new DTipLabel(tr("Click the option in boot menu to set it as the first boot, and drag and drop a picture to change the background"));
    backgroundLabel->setWordWrap(true);
    backgroundLabel->setContentsMargins(5, 0, 10, 0);
    backgroundLabel->setAlignment(Qt::AlignLeft);
#ifndef DCC_DISABLE_GRUB_THEME
    DTipLabel *themeLbl = new DTipLabel(tr("Switch theme on to view it in boot menu"));
    themeLbl->setWordWrap(true);
    themeLbl->setContentsMargins(5, 0, 10, 0);
    themeLbl->setAlignment(Qt::AlignLeft);
#endif
    groupOther->appendItem(m_bootDelay);
    groupOther->setSpacing(List_Interval);
#ifndef DCC_DISABLE_GRUB_THEME
    groupOther->appendItem(m_theme);
#endif
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(List_Interval);
    layout->addWidget(m_background);
    layout->addSpacing(List_Interval);
    layout->addWidget(backgroundLabel);
    layout->addSpacing(List_Interval);
    layout->addWidget(groupOther);
#ifndef DCC_DISABLE_GRUB_THEME
    layout->addWidget(themeLbl);
#endif
    layout->addStretch();
    layout->setContentsMargins(ThirdPageContentsMargins);
    setLayout(layout);
    setWindowTitle(tr("Boot Menu"));

#ifndef DCC_DISABLE_GRUB_THEME
    connect(m_theme, &SwitchWidget::checkedChanged, this, &BootWidget::enableTheme);
#endif
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
#ifndef DCC_DISABLE_GRUB_THEME
    connect(model, &CommonInfoModel::themeEnabledChanged, m_theme, &SwitchWidget::setChecked);
#endif
    connect(model, &CommonInfoModel::defaultEntryChanged, this, &BootWidget::setDefaultEntry);
    connect(model, &CommonInfoModel::updatingChanged, m_updatingLabel, &SmallLabel::setVisible);
    connect(model, &CommonInfoModel::entryListsChanged, this, &BootWidget::setEntryList);
    connect(model, &CommonInfoModel::themeEnabledChanged, m_background, &CommonBackgroundItem::setThemeEnable);
    connect(model, &CommonInfoModel::backgroundChanged, m_background, &CommonBackgroundItem::updateBackground);

    // modified by wuchuanfei 20190909 for 8613
    if (model->entryLists().count() == 1) {
        m_bootDelay->setChecked(false);
    } else {
        m_bootDelay->setChecked(model->bootDelay());
    }
#ifndef DCC_DISABLE_GRUB_THEME
    m_theme->setChecked(model->themeEnabled());
#endif
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
        item->setCheckable(false); // for Bug 2449
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

void BootWidget::resizeEvent(QResizeEvent *event)
{
    auto w = event->size().width();
    m_listLayout->setContentsMargins(w * 0.2, 0, w * 0.2, 0);
}
