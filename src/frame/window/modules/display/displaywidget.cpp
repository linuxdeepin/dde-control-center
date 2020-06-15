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

#include "displaywidget.h"
#include "interface/namespace.h"
#include "window/utils.h"
#include "widgets/multiselectlistview.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"

#include <DStyleOption>

#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE
#define GSETTINGS_SHOW_MUTILSCREEN "show-multiscreen"

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent)
    , m_rotate(new DFloatingButton(this))
    , m_centralLayout(new QVBoxLayout(this))
    , m_menuList(new dcc::widgets::MultiSelectListView(this))
    , m_multiModel(new QStandardItemModel(this))
    , m_singleModel(new QStandardItemModel(this))
{
    setObjectName("Display");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_centralLayout->setSpacing(10);
    m_centralLayout->setMargin(0);
    setLayout(m_centralLayout);

    initMenuUI();
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;
    m_isMultiScreen = model->monitorList().size() > 1;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configCreated, this, &DisplayWidget::requestShowCustomConfigPage);

    //确保第一次进入onMonitorListChanged会命中一个判断
    m_isMultiScreen = model->monitorList().size() <= 1;
    onMonitorListChanged();
    if (model->isRefreshRateEnable() == false) {
        for (int i = 0; i < m_singleModel->rowCount(); i++) {
            if (m_singleModel->item(i)->text() == tr("Refresh Rate")) {
                m_singleModel->removeRow(i);
            }
        }
    }
}

int DisplayWidget::showPath(const QString &path)
{
    if (((path == "Resolution" || path == "Refresh Rate") && m_model->monitorList().size() > 1) ||
        path == "Customize") {
        Q_EMIT this->requestShowCustomConfigPage();
        m_currentIdx = m_menuList->model()->index(0, 0);
        m_menuList->setCurrentIndex(m_currentIdx);
        return 0;
    }

    auto menuList = m_isMultiScreen ? m_multMenuList : m_singleMenuList;
    for (int i = 0; i < menuList.size(); ++i) {
        auto menu = menuList[i];
        if (tr(path.toStdString().c_str()) == menu.menuText) {
            menu.method.invoke(this);
            m_currentIdx = m_menuList->model()->index(i, 0);
            m_menuList->setCurrentIndex(m_currentIdx);
            return 0;
        }
    }

    return -1;
}

QPoint DisplayWidget::getRotateBtnPos()
{
    QPoint curPoint = mapToGlobal(m_rotate->pos());
    return QPoint(curPoint.x() + m_rotate->width() / 2, curPoint.y() + m_rotate->height() / 2);
}

void DisplayWidget::onMonitorListChanged()
{
    const auto mons = m_model->monitorList();

    m_rotate->setVisible(mons.size() <= 1);
    if (m_isMultiScreen && mons.size() <= 1) {
        m_isMultiScreen = false;
        m_menuList->setModel(m_singleModel);
        m_rotate->show();
        onMenuClicked(m_menuList->model()->index(0, 0));
    } else if (!m_isMultiScreen && mons.size() > 1) {
        m_isMultiScreen = true;
        m_menuList->setModel(m_multiModel);
        m_rotate->hide();
        onMenuClicked(m_menuList->model()->index(0, 0));
    }
}

void DisplayWidget::initMenuUI()
{
    m_multMenuList = {
        //~ contents_path /display/Multiple Displays
        {tr("Multiple Displays"), "dcc_display", QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiScreenPage)},
        //~ contents_path /display/Brightness
        {tr("Brightness"), "dcc_brightness", QMetaMethod::fromSignal(&DisplayWidget::requestShowBrightnessPage)},
    };
    m_singleMenuList = {
        //~ contents_path /display/Resolution
        {tr("Resolution"), "dcc_resolution", QMetaMethod::fromSignal(&DisplayWidget::requestShowResolutionPage)},
        {tr("Brightness"), "dcc_brightness", QMetaMethod::fromSignal(&DisplayWidget::requestShowBrightnessPage)},
    };

    if (!IsServerSystem) {
        //~ contents_path /display/Display Scaling
        MenuMethod scaleMenu = {tr("Display Scaling"), "dcc_screen",
                                QMetaMethod::fromSignal(&DisplayWidget::requestShowScalingPage)
                               };
        m_multMenuList << scaleMenu;
        m_singleMenuList << scaleMenu;
    }

    //~ contents_path /display/Refresh Rate
    MenuMethod refreshMenu = {tr("Refresh Rate"), "dcc_refresh_rate",
                              QMetaMethod::fromSignal(&DisplayWidget::requestShowRefreshRatePage)
                             };
    m_singleMenuList << refreshMenu;

    if (m_model && !m_model->touchscreenList().isEmpty()) {
        //~ contents_path /display/Touch Screen
        MenuMethod touchscreenMenu = {tr("Touch Screen"),
                                      "dcc_touchscreen",
                                      QMetaMethod::fromSignal(&DisplayWidget::requestShowTouchscreenPage)};
        m_multMenuList << touchscreenMenu;
        m_singleMenuList << touchscreenMenu;
    }

    m_displaySetting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);
    m_isShowMultiscreen = m_displaySetting->get(GSETTINGS_SHOW_MUTILSCREEN).toBool();
    if (!m_isShowMultiscreen) {
        m_multMenuList.removeAt(0);
        MenuMethod multiRefreshMenu = {tr("Refresh Rate"), "dcc_refresh_rate",
                                  QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiRefreshRatePage)
                                 };
        MenuMethod multiResoMenu = {tr("Resolution"), "dcc_resolution",
                                  QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiResolutionPage)
                                 };
        m_multMenuList << multiResoMenu << multiRefreshMenu;
        m_singleMenuList[0] = {tr("Resolution"), "dcc_resolution", QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiResolutionPage)};
        m_singleMenuList[3] = {tr("Refresh Rate"), "dcc_resolution", QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiRefreshRatePage)};
    }

    DStandardItem *btn{nullptr};
    for (auto menu : m_multMenuList) {
        btn = new DStandardItem(menu.menuText);
        btn->setData(VListViewItemMargin, Dtk::MarginsRole);
        btn->setIcon(QIcon::fromTheme(menu.iconName));
        m_multiModel->appendRow(btn);
    }

    for (auto menu : m_singleMenuList) {
        btn = new DStandardItem(menu.menuText);
        btn->setData(VListViewItemMargin, Dtk::MarginsRole);
        btn->setIcon(QIcon::fromTheme(menu.iconName));
        m_singleModel->appendRow(btn);
    }

    m_menuList->setAccessibleName("List_displaymenulist");
    m_menuList->setEditTriggers(DListView::NoEditTriggers);
    m_menuList->setFrameShape(QFrame::NoFrame);
    m_menuList->setViewportMargins(ScrollAreaMargins);
    m_menuList->setIconSize(ListViweIconSize);
    m_centralLayout->addWidget(m_menuList, 1);
    connect(m_menuList, &QListView::clicked, this, &DisplayWidget::onMenuClicked);
    connect(m_menuList, &DListView::activated, m_menuList, &QListView::clicked);

    //  m_centralLayout->addStretch(1);
    m_rotate->setIcon(QIcon::fromTheme("dcc_rotate"));
    //~ contents_path /display/Resolution
    m_rotate->setToolTip(tr("Rotate Screen"));

    m_centralLayout->addWidget(m_rotate, 0, Qt::AlignCenter);
    connect(m_rotate, &DFloatingButton::clicked, this, &DisplayWidget::requestRotate);
}

void DisplayWidget::onMenuClicked(const QModelIndex &idx)
{
    if (idx == m_currentIdx) return;

    m_currentIdx = idx;
    m_menuList->setCurrentIndex(m_currentIdx);
    if (m_isMultiScreen) {
        m_multMenuList[idx.row()].method.invoke(this);
    } else {
        m_singleMenuList[idx.row()].method.invoke(this);
    }
    m_menuList->resetStatus(idx);
}
