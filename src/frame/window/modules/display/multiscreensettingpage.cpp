/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "multiscreensettingpage.h"
#include "modules/display/displaymodel.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QPushButton>

using namespace DCC_NAMESPACE::display;
using namespace dcc::widgets;
using namespace dcc::display;
DWIDGET_USE_NAMESPACE

MultiScreenSettingPage::MultiScreenSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_modeList(new DListView)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(ThirdPageContentsMargins);

    m_modeList->setEditTriggers(DListView::NoEditTriggers);
    m_modeList->setSelectionMode(DListView::NoSelection);
    m_modeList->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_listModel = new QStandardItemModel(this);
    m_modeList->setModel(m_listModel);

    mainLayout->addWidget(m_modeList);

///    //~ contents_path /display/Customize
    QPushButton *btn = new QPushButton(tr("Custom Settings"), this);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(btn);

    setLayout(mainLayout);

    connect(btn, &QPushButton::clicked, this, &MultiScreenSettingPage::onCustomClicked);
    connect(m_modeList, &DListView::clicked, this, &MultiScreenSettingPage::onItemClicked);
}

void MultiScreenSettingPage::setModel(dcc::display::DisplayModel *model)
{
    m_model = model;

    initModeList();
    connect(m_model, &DisplayModel::displayModeChanged,
            this, &MultiScreenSettingPage::onDisplayModeChanged);
    connect(m_model, &DisplayModel::primaryScreenChanged,
            this, &MultiScreenSettingPage::onDisplayModeChanged);
}

void MultiScreenSettingPage::onItemClicked(const QModelIndex &index)
{
    switch (index.row()) {
    case 0:
        Q_EMIT requestDuplicateMode();
        return;
    case 1:
        Q_EMIT requestExtendMode();
        return;
    default:
        break;
    }

    if (index.row() == m_listModel->rowCount() - 1) {
        Q_EMIT requestCustomMode();
        return;
    }

    auto moniName = m_model->monitorList()[index.row() - 2]->name();
    Q_EMIT requestOnlyMonitor(moniName);
}

void MultiScreenSettingPage::onDisplayModeChanged()
{
    if (m_currIdx.isValid())
        m_listModel->setData(m_currIdx, Qt::Unchecked, Qt::CheckStateRole);

    switch (m_model->displayMode()) {
    case MERGE_MODE:
        m_currIdx = m_listModel->index(0, 0);
        break;
    case EXTEND_MODE:
        m_currIdx = m_listModel->index(1, 0);
        break;
    case SINGLE_MODE: {
        for (int tidx = 0; tidx < m_model->monitorList().size(); ++tidx) {
            auto m = m_model->monitorList()[tidx];
            if (m == m_model->primaryMonitor()) {
                m_currIdx = m_listModel->index(tidx + 2, 0);
                break;
            }
        }
        break;
    }
    case CUSTOM_MODE:
        m_currIdx = m_listModel->index(m_listModel->rowCount() - 1, 0);
        break;
    }

    if (m_currIdx.isValid())
        m_listModel->setData(m_currIdx, Qt::Checked, Qt::CheckStateRole);
}

void MultiScreenSettingPage::initModeList()
{
    QStringList titleList;
    titleList << tr("Duplicate") << tr("Extend");

    QStringList subTitleList;
    subTitleList << tr("Show the same image on other screens")
                 << tr("Expand the desktop across the screens");

    QStringList iconList;
    iconList << "dcc_display_copy" << "dcc_display_expansion";

    auto moniList = m_model->monitorList();
    for (int i = 0; i < moniList.size(); ++i) {
        auto moniNmae = moniList[i]->name();
        titleList << tr("%1 only").arg(moniNmae);
        subTitleList << tr("Show the screen content only on %1").arg(moniNmae);
        //change icon by displayer name
        //Desktop:HDMI-A-0 VGA-0 DP-1   Notebook:eDP-0
        if(moniNmae.contains("eDP-")) {
            iconList << "dcc_display_vga1";
        } else {
            iconList << "dcc_display_lvds1";
        }
    }

    titleList << tr("Customize");
    subTitleList << tr("Configure the display according to your needs");
    iconList << "dcc_display_custom";

    for (int idx = 0; idx < titleList.size(); ++idx) {
        auto item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(iconList[idx]));
        item->setText(titleList[idx]);

        auto *subTitleAction = new DViewItemAction;
        subTitleAction->setText(subTitleList[idx]);
        subTitleAction->setFontSize(DFontSizeManager::T6);
        subTitleAction->setTextColorRole(DPalette::TextTips);
        item->setTextActionList({subTitleAction});
        m_listModel->appendRow(item);
    }

    onDisplayModeChanged();
}

void MultiScreenSettingPage::onCustomClicked()
{
    Q_EMIT requsetRecord();

    auto displayMode = m_model->displayMode();

    if (displayMode == CUSTOM_MODE && m_model->config() == m_model->DDE_Display_Config) {
        Q_EMIT requestCustomDiglog();
    } else {
        Q_EMIT requsetCreateConfig(m_model->DDE_Display_Config);
    }
}
