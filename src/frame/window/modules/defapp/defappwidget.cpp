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

#include "defappwidget.h"
#include "window/utils.h"
#include "widgets/multiselectlistview.h"
#include "window/insertplugin.h"
#include "window/gsettingwatcher.h"

#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>

using namespace DCC_NAMESPACE::defapp;

DWIDGET_USE_NAMESPACE

DefaultAppsWidget::DefaultAppsWidget(QWidget *parent)
    : QWidget(parent)
    , m_defAppCatView(new dcc::widgets::MultiSelectListView(this))
    , m_centralLayout(new QVBoxLayout(this))
{
    setObjectName("Defapp");
    m_defAppCatView->setAccessibleName("List_defaultappcatList");
    m_defAppCatView->setResizeMode(QListView::Adjust);
    m_defAppCatView->setMovement(QListView::Static);
    m_defAppCatView->setFrameShape(QFrame::NoFrame);
    m_defAppCatView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_defAppCatView->setViewportMargins(ScrollAreaMargins);
    m_defAppCatView->setIconSize(ListViweIconSize);

    QStringList titles;
    //~ contents_path /defapp/Webpage
    //~ child_page_hide Webpage
    titles << tr("Webpage");
    //~ contents_path /defapp/Mail
    //~ child_page_hide Mail
    titles << tr("Mail");
    //~ contents_path /defapp/Text
    //~ child_page_hide Text
    titles << tr("Text");
    //~ contents_path /defapp/Music
    //~ child_page_hide Music
    titles << tr("Music");
    //~ contents_path /defapp/Video
    //~ child_page_hide Video
    titles << tr("Video");
    //~ contents_path /defapp/Picture
    //~ child_page_hide Picture
    titles << tr("Picture");
    //~ contents_path /defapp/Terminal
    //~ child_page_hide Terminal
    titles << tr("Terminal");

    /*以下三行为默认程序模块服务，由于每个cpp只能有一种翻译，故将注释分配到其他地方*/
    //~ contents_path /defapp/Webpage/Add Application
    //~ child_page Webpage
    tr("Add Application");

    QStringList icons;
    icons << "dcc_browser"
          << "dcc_mail"
          << "dcc_text"
          << "dcc_music"
          << "dcc_video"
          << "dcc_photo"
          << "dcc_terminal";

    QStringList defappGsetting;
    defappGsetting << "defappWebpage"
          << "defappMail"
          << "defappText"
          << "defappMusic"
          << "defappVideo"
          << "defappPicture"
          << "defappTerminal";


    //Initialize second page view and model
    m_itemModel = new QStandardItemModel(this);

    if(InsertPlugin::instance()->updatePluginInfo("defapp"))
        InsertPlugin::instance()->pushPlugin(m_itemModel,m_itemList);

    m_defAppCatView->setModel(m_itemModel);
    //show default browser app
    m_lastIndex = m_itemModel->indexFromItem(m_itemModel->item(0));
    m_defAppCatView->resetStatus(m_lastIndex);

    m_centralLayout->setMargin(0);
    m_centralLayout->addWidget(m_defAppCatView);

    connect(m_defAppCatView, &QListView::clicked, this, &DefaultAppsWidget::onCategoryClicked);
    connect(m_defAppCatView, &DListView::activated, m_defAppCatView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row, const QString & name) {
        //不是本模块配置不响应
        if (!configContent(name))
            return ;
        bool isAllHiden = true;
        for (int i = 0; i < m_itemModel->rowCount(); i++) {
            if (!m_defAppCatView->isRowHidden(i))
                isAllHiden = false;
        }

        if (m_defAppCatView->selectionModel()->selectedRows().size() > 0) {
            int index = m_defAppCatView->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHiden) {
            m_lastIndex = QModelIndex();
            m_defAppCatView->clearSelection();
        }
    });

    for (int i = 0; i < icons.size(); i++) {
        DStandardItem *item = new DStandardItem(QIcon::fromTheme(icons.at(i)), titles.at(i));

        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_itemModel->appendRow(item);

        m_itemList.push_back({icons.at(i),titles.at(i),QMetaMethod::fromSignal(&DefaultAppsWidget::requestCategoryClicked),nullptr,defappGsetting.at(i)});
        GSettingWatcher::instance()->bind(defappGsetting.at(i), m_defAppCatView, item);
    }

    setAccessibleName(tr("Default Applications"));
}

DefaultAppsWidget::~DefaultAppsWidget()
{

}

void DefaultAppsWidget::setCurrentIndex(int row)
{
    if (row > m_defAppCatView->model()->rowCount())
        row = 0;

    m_defAppCatView->setCurrentIndex(qobject_cast<QStandardItemModel *>(m_defAppCatView->model())->index(row, 0));
}

void DefaultAppsWidget::onCategoryClicked(const QModelIndex &index) {
    if (m_lastIndex == index) return;

    m_lastIndex = index;
    qDebug() << "DefaultAppsWidget clicked row " << index.row();

    if(m_itemList[index.row()].plugin) {
        m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].plugin);
    } else {
        m_itemList[index.row()].itemSignal.invoke(this, Qt::ConnectionType::DirectConnection, Q_ARG(int, index.row()));
    }

    m_defAppCatView->resetStatus(index);
}

void DefaultAppsWidget::showDefaultWidget()
{
    for (int i = 0; i < m_itemModel->rowCount(); i++) {
        if (!m_defAppCatView->isRowHidden(i)) {
            m_defAppCatView->activated(m_itemModel->index(i, 0));
            break;
        }
    }
}

bool DefaultAppsWidget::configContent(const QString &configName)
{
    for (auto m : m_itemList) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}
