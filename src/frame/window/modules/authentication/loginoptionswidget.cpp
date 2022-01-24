/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>
 *
 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "loginoptionswidget.h"
#include "widgets/multiselectlistview.h"
#include "window/gsettingwatcher.h"
#include "window/utils.h"

#include <QBoxLayout>
#include <QScroller>
#include <QStandardItemModel>


using namespace dcc::widgets;
using namespace DCC_NAMESPACE::authentication;

LoginOptionsWidget::LoginOptionsWidget(QWidget *parent)
    : QWidget (parent)
    , m_deviceListView(new dcc::widgets::MultiSelectListView(this))
    , m_deviceItemModel(new QStandardItemModel(this))
{
    setObjectName("Suthentication");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    initUI();
    initMembers();
    initConnections();
}

LoginOptionsWidget::~LoginOptionsWidget()
{
    if (m_deviceItemModel) {
        m_deviceItemModel->clear();
        m_deviceItemModel->deleteLater();
        m_deviceItemModel = nullptr;
    }
}

int LoginOptionsWidget::showPath(const QString &path)
{
    auto getIndex = [ = ](const QString &name){
        for(int i = 0; i < m_itemList.size(); i++) {
            if (name == "Fingerprint" && m_itemList[i].itemText == tr("Fingerprint")) {
                return i;
            }

            if (name == "Face" && m_itemList[i].itemText == tr("Face")) {
                return i;
            }

            if (name == "Iris" && m_itemList[i].itemText == tr("Iris")) {
                return i;
            }
        }
        return 0;
    };

    int index = getIndex(path);
    m_itemList[ index ].itemSignal.invoke(this);
    m_currentIndex = m_deviceListView->model()->index(index, 0);
    m_deviceListView->setCurrentIndex(m_currentIndex);
    return index;
}

void LoginOptionsWidget::showDefaultWidget()
{
    for (int i = 0; i < m_deviceListView->model()->rowCount(); i++) {
        if (!m_deviceListView->isRowHidden(i)) {
            m_deviceListView->activated(m_deviceListView->model()->index(i, 0));
            break;
        }
    }
}

void LoginOptionsWidget::onItemClicked(const QModelIndex &index)
{
    if (index == m_currentIndex)
        return;

    m_currentIndex = index;
    m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].plugin ? m_itemList[index.row()].plugin : this);
    m_deviceListView->resetStatus(index);
}

void LoginOptionsWidget::initUI()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setMargin(0);
    mainContentLayout->addWidget(m_deviceListView);

    m_deviceListView->setAccessibleName("List_userlist");
    m_deviceListView->setFrameShape(QFrame::NoFrame);
    m_deviceListView->setViewportMargins(ScrollAreaMargins);
    m_deviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_deviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_deviceListView->setEditTriggers(QListView::NoEditTriggers);
    m_deviceListView->setDragEnabled(false);
    m_deviceListView->setIconSize(QSize(30, 30));
    m_deviceListView->setLayoutDirection(Qt::LeftToRight);
    m_deviceListView->setModel(m_deviceItemModel);

    QScroller::grabGesture(m_deviceListView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_deviceListView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    m_currentIndex = m_deviceListView->currentIndex();
    m_deviceListView->resetStatus(m_currentIndex);
    setLayout(mainContentLayout);
}

void LoginOptionsWidget::initMembers()
{
    //~ contents_path /authentication/Fingerprint
    m_itemList.append({"dcc_fingerprint", tr("Fingerprint"), QMetaMethod::fromSignal(&LoginOptionsWidget::requestShowFingerDetail), nullptr, "authenticationFinger"});
    //~ contents_path /authentication/Face
    m_itemList.append({"dcc_faceid", tr("Face"), QMetaMethod::fromSignal(&LoginOptionsWidget::requestShowFaceIdDetail), nullptr, "authenticationFace"});
    //~ contents_path /authentication/Iris
    m_itemList.append({"dcc_iris", tr("Iris"), QMetaMethod::fromSignal(&LoginOptionsWidget::requestShowIrisDetail), nullptr, "authenticationIris"});

    for (auto mm : m_itemList) {
        DStandardItem *item = new DStandardItem(mm.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setIcon(QIcon::fromTheme(mm.itemIcon));
        m_deviceItemModel->appendRow(item);
        GSettingWatcher::instance()->bind(mm.gsettingsName, m_deviceListView, item);
    }
}

void LoginOptionsWidget::initConnections()
{
    connect(m_deviceListView, &QListView::clicked, this, &LoginOptionsWidget::onItemClicked);
    connect(m_deviceListView, &DListView::activated, m_deviceListView, &QListView::clicked);

    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row, const QString & name) {
        //不是本模块配置不响应
        if (!configContent(name))
            return ;
        bool isAllHidden = true;
        for (int i = 0; i < m_deviceItemModel->rowCount(); i++) {
            if (!m_deviceListView->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_deviceListView->selectionModel()->selectedRows().size() > 0) {
            int index = m_deviceListView->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHidden) {
            m_currentIndex = QModelIndex();
            m_deviceListView->clearSelection();
        }
    });
}

bool LoginOptionsWidget::configContent(const QString &configName)
{
    for (auto m : m_itemList) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}
