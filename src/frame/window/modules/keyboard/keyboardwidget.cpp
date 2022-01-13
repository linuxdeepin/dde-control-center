/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#include "keyboardwidget.h"
#include "window/insertplugin.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/multiselectlistview.h"
#include "window/gsettingwatcher.h"

#include <QVBoxLayout>
#include <QList>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc::widgets;
KeyboardWidget::KeyboardWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("Keyboard");
    m_contentLayout = new QVBoxLayout(this);
    m_contentLayout->setMargin(0);
    m_keyboardListView = new dcc::widgets::MultiSelectListView(this);
    m_contentLayout->addWidget(m_keyboardListView);
    init();
    setLayout(m_contentLayout);
}

KeyboardWidget::~KeyboardWidget()
{

}

void KeyboardWidget::init()
{
    m_listviewModel = new QStandardItemModel(m_keyboardListView);
    m_keyboardListView->setAccessibleName("List_keyboardlist");
    m_keyboardListView->setFrameShape(QFrame::NoFrame);
    m_keyboardListView->setResizeMode(QListView::Adjust);
    m_keyboardListView->setMovement(QListView::Static);
    m_keyboardListView->setModel(m_listviewModel);
    m_keyboardListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_keyboardListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_keyboardListView->setCurrentIndex(m_listviewModel->index(0, 0));
    m_keyboardListView->setViewportMargins(ScrollAreaMargins);
    m_keyboardListView->setIconSize(ListViweIconSize);
    m_keyboardListView->resetStatus(m_lastIndex);

    QList<QPair<QString, QString>> menuIconText;
    menuIconText = {
        { "dcc_general_purpose", tr("General")},
        { "dcc_keyboard", tr("Keyboard Layout")},
        { "dcc_language", tr("System Language")},
        { "dcc_hot_key", tr("Shortcuts")}
    };

    // gsetting配置名字与menuIconText顺序对应，有相关顺序调整需对应修改
    m_itemList.append({menuIconText[0].first, menuIconText[0].second, QMetaMethod::fromSignal(&KeyboardWidget::showGeneralSetting), nullptr, "keyboardGeneral"});
    m_itemList.append({menuIconText[1].first, menuIconText[1].second, QMetaMethod::fromSignal(&KeyboardWidget::showKBLayoutSetting), nullptr, "keyboardLayout"});
    m_itemList.append({menuIconText[2].first, menuIconText[2].second, QMetaMethod::fromSignal(&KeyboardWidget::showSystemLanguageSetting), nullptr, "keyboardLanguage"});
    m_itemList.append({menuIconText[3].first, menuIconText[3].second, QMetaMethod::fromSignal(&KeyboardWidget::showShortCutSetting), nullptr, "keyboardShortcuts"});

    int index = 0;
    DStandardItem *keyboardItem = nullptr;
    for (auto it = menuIconText.cbegin(); it != menuIconText.cend(); ++it) {
        keyboardItem = new DStandardItem(QIcon::fromTheme(it->first), it->second);
        keyboardItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_listviewModel->appendRow(keyboardItem);
        GSettingWatcher::instance()->bind(m_itemList.at(index).gsettingsName, m_keyboardListView, keyboardItem);
        ++index;
    }

    if(InsertPlugin::instance()->updatePluginInfo("keyboard"))
        InsertPlugin::instance()->pushPlugin(m_listviewModel,m_itemList);

    connect(m_keyboardListView, &DListView::clicked, this, &KeyboardWidget::onItemClick);
    connect(m_keyboardListView, &DListView::activated, m_keyboardListView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [ = ](const int row, const QString & name) {
        //不是本模块配置不响应
        if (!configContent(name))
            return ;
        bool isAllHidden = true;
        for (int i = 0; i < m_keyboardListView->model()->rowCount(); i++) {
            if (!m_keyboardListView->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_keyboardListView->selectionModel()->selectedRows().size() > 0) {
            const int index = m_keyboardListView->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHidden) {
            m_lastIndex = QModelIndex();
            m_keyboardListView->clearSelection();
        }
    });
}

void KeyboardWidget::initSetting(const int settingIndex)
{
    m_keyboardListView->setCurrentIndex(m_listviewModel->index(settingIndex, 0));
    m_keyboardListView->clicked(m_listviewModel->index(settingIndex, 0));
}

void KeyboardWidget::setDefaultWidget()
{
    for(int i = 0; i < m_keyboardListView->model()->rowCount(); i++) {
        if (!m_keyboardListView->isRowHidden(i)) {
            m_keyboardListView->activated(m_keyboardListView->model()->index(i, 0));
            break;
        }
    }
}

int KeyboardWidget::showPath(const QString &path)
{
    for (int i = 0; i < m_itemList.size(); ++i) {
        auto menu = m_itemList[i];
        if (tr(path.toStdString().c_str()) == menu.itemText) {
            menu.itemSignal.invoke(this);
            m_lastIndex = m_listviewModel->index(i, 0);
            m_keyboardListView->setCurrentIndex(m_lastIndex);
            return 0;
        }

        //正常情况应该使用 "Input Methods" 进行判断，因为输入法写错了因此这里需要根据输入法错误写法适配
        if (menu.plugin && path == "Manage Input Methods") {
            menu.itemSignal.invoke(menu.plugin);
            m_lastIndex = m_listviewModel->index(i, 0);
            m_keyboardListView->setCurrentIndex(m_lastIndex);
            return 0;
        }
    }

    return -1;
}

void KeyboardWidget::onItemClick(const QModelIndex &index)
{
    if (index == m_lastIndex) {
        return;
    }

    m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].plugin ? m_itemList[index.row()].plugin : this);

    m_lastIndex = index;
    m_keyboardListView->resetStatus(index);
}

bool KeyboardWidget::configContent(const QString &configName)
{
    for (auto m : m_itemList) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}
