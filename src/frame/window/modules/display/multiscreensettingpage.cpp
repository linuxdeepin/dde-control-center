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

    m_modeList->installEventFilter(this);
    m_modeList->setEditTriggers(DListView::NoEditTriggers);
    mainLayout->addWidget(m_modeList);

    //~ contents_path /display/Customize
    QPushButton *btn = new QPushButton(tr("Customize"), this);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(btn);

    setLayout(mainLayout);

    connect(btn, &QPushButton::clicked, this, &MultiScreenSettingPage::onCustomClicked);
    connect(m_modeList, &DListView::clicked, this, &MultiScreenSettingPage::onItemClicked);
}

void MultiScreenSettingPage::setModel(dcc::display::DisplayModel *model)
{
    m_model = model;
    m_modeList->setModel(new QStandardItemModel());

    initModeList();
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

    auto moniName = m_model->monitorList()[index.row() - 2]->name();
    Q_EMIT requestOnlyMonitor(moniName);
}

void MultiScreenSettingPage::initModeList()
{
    auto listModel = qobject_cast<QStandardItemModel * >(m_modeList->model());
    Q_ASSERT(listModel);

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
        iconList << (i % 2 ? "dcc_display_vga1" : "dcc_display_lvds1");
    }

    for (int idx = 0; idx < titleList.size(); ++idx) {
        auto *titleAction = new DViewItemAction;
        titleAction->setText(titleList[idx]);

        auto *subTitleAction = new DViewItemAction;
        subTitleAction->setText(subTitleList[idx]);

        DViewItemActionList actionList;
        actionList << titleAction << subTitleAction;

        auto item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(iconList[idx]));
        item->setTextActionList(actionList);
        listModel->appendRow(item);
    }
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
