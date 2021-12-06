/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "systeminfowidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/multiselectlistview.h"

#include <DStandardItem>

#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QIcon>
#include <QList>
#include <QSettings>
#include <QDebug>
#include <QDir>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoWidget::SystemInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_listView(new dcc::widgets::MultiSelectListView(this))
    , m_itemModel(new QStandardItemModel(this))
{
    initWidget();
    initData();
}

SystemInfoWidget::~SystemInfoWidget()
{

}

void SystemInfoWidget::initWidget()
{
    m_listView->setAccessibleName("List_ systeminfomenulist");
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
    m_listView->setModel(m_itemModel);
    m_listView->setViewportMargins(ScrollAreaMargins);
    m_listView->setIconSize(ListViweIconSize);
    m_listView->setItemSize(ListViweSysItemSize);

    m_mainContentLayout->setMargin(0);
    m_mainContentLayout->addWidget(m_listView);

    setLayout(m_mainContentLayout);
}

void SystemInfoWidget::initData()
{
    m_itemList = {
        {"dcc_on_sel", tr("About This PC"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowAboutNative), nullptr, "aboutThisPc"},
        //~ contents_path /systeminfo/Edition License
        {"dcc_version", tr("Edition License"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowVersionProtocol), nullptr, "editionLicense"},
        //~ contents_path /systeminfo/End User License Agreement
        {"dcc_protocol", tr("End User License Agreement"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowEndUserLicenseAgreement), nullptr, "endUserLicenseAgreement"},
        //~ contents_path /systeminfo/Privacy Policy
        {"dcc_privacy_policy", tr("Privacy Policy"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowPrivacyPolicy), nullptr, "privacyPolicy"}
    };

    for (auto m : m_itemList) {
        DStandardItem *item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(m.itemIcon));
        item->setText(m.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_itemModel->appendRow(item);
        GSettingWatcher::instance()->bind(m.gsettingsName, m_listView, item);
    }

   if(InsertPlugin::instance()->needPushPlugin("systeminfo"))
        InsertPlugin::instance()->pushPlugin(m_itemModel,m_itemList);

    connect(m_listView, &DListView::clicked, this, &SystemInfoWidget::onListClicked);
    connect(m_listView, &DListView::activated, m_listView, &DListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, &SystemInfoWidget::onRequestUpdateSecondMenu);
}

DListView *SystemInfoWidget::getSystemListViewPointer()
{
    return m_listView;
}

void SystemInfoWidget::showDefaultWidget()
{
    if (!m_listView)
        return;

    for(int i = 0; i < m_listView->model()->rowCount(); i++) {
        if (!m_listView->isRowHidden(i)) {
            m_listView->activated(m_listView->model()->index(i, 0));
            break;
        }
    }
}

bool SystemInfoWidget::configContent(const QString &configName)
{
    for (auto m : m_itemList) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}

int SystemInfoWidget::showPath(const QString &path)
{
    for (int i = 0; i < m_itemList.size(); ++i) {
        auto menu = m_itemList[i];
        if (tr(path.toStdString().c_str()) == menu.itemText) {
            setCurrentIndex(i);
            return 0;
        }
    }

    if (path == "Backup and Restore") {
        setCurrentIndex(m_itemModel->rowCount() - 1);
        return 0;
    }

    return -1;
}

void SystemInfoWidget::setCurrentIndex(int index)
{
    QModelIndex mindex = m_itemModel->index(index, 0);
    m_listView->setCurrentIndex(mindex);
    Q_EMIT m_listView->clicked(mindex);
}

void SystemInfoWidget::onRequestUpdateSecondMenu(int row, const QString & name)
{
    //不是本模块配置不响应
    if (!configContent(name))
        return;
    bool isAllHiden = true;
    for (int i = 0; i < m_itemModel->rowCount(); i++) {
        if (!m_listView->isRowHidden(i))
            isAllHiden = false;
    }

    if (m_listView->selectionModel()->selectedRows().size() > 0) {
        int index = m_listView->selectionModel()->selectedRows()[0].row();
        Q_EMIT requestUpdateSecondMenu(index == row);
    } else {
        Q_EMIT requestUpdateSecondMenu(false);
    }

    if (isAllHiden) {
        m_lastIndex = QModelIndex();
        m_listView->clearSelection();
    }
}

void SystemInfoWidget::onListClicked(const QModelIndex &index)
{
    if (m_lastIndex == index)
	    return;

    m_lastIndex = index;
    m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].pulgin ? m_itemList[index.row()].pulgin : this);
    m_listView->resetStatus(index);
}
