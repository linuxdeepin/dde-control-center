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
#include "window/insertplugin.h"
#include "widgets/multiselectlistview.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"

#include <DApplicationHelper>
#include <DStyleOption>

#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE
#define GSETTINGS_SHOW_MUTILSCREEN "show-multiscreen"
#define GSETTINGS_BRIGHTNESS_ENABLE "brightness-enable"

DisplayWidget::DisplayWidget(dcc::display::DisplayModel *model, QWidget *parent)
    : QWidget(parent)
    , m_rotate(new DFloatingButton(this))
    , m_centralLayout(new QVBoxLayout(this))
    , m_menuList(new dcc::widgets::MultiSelectListView(this))
    , m_multiModel(new QStandardItemModel(this))
    , m_singleModel(new QStandardItemModel(this))
    , m_displaySetting(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
{
    m_isShowMultiscreen = m_displaySetting->get(GSETTINGS_SHOW_MUTILSCREEN).toBool();
    m_model = model;
    setObjectName("Display");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_centralLayout->setSpacing(10);
    m_centralLayout->setMargin(0);
    setLayout(m_centralLayout);

    initMenuUI();
}

void DisplayWidget::setModel()
{
    m_isMultiScreen = m_model->monitorList().size() > 1;

    // 平板一期只支持单屏，多屏显示置false
#ifdef USE_TABLET
    m_isShowMultiscreen = false;
#endif

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configCreated, this, &DisplayWidget::requestShowCustomConfigPage);

    //确保第一次进入onMonitorListChanged会命中一个判断
    m_isMultiScreen = m_model->monitorList().size() <= 1;
    onMonitorListChanged();
    if (m_model->isRefreshRateEnable() == false) {
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
        Q_EMIT requestShowCustomConfigPage();
        m_currentIdx = m_menuList->model()->index(0, 0);
        m_menuList->setCurrentIndex(m_currentIdx);
        return 0;
    }

    auto menuList = m_isMultiScreen ? m_multMenuList : m_singleMenuList;
    for (int i = 0; i < menuList.size(); ++i) {
        auto menu = menuList[i];
        if (tr(path.toStdString().c_str()) == menu.itemText) {
            menu.itemSignal.invoke(this);
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
    // 配置文件关闭多屏显示模式按显示菜单显示单屏配置
    if ((m_isMultiScreen && mons.size() <= 1) || !m_isShowMultiscreen) {
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
    m_multiModel->clear();
    m_singleModel->clear();

    m_multMenuList = {
        //~ contents_path /display/Multiple Displays
        {"dcc_display", tr("Multiple Displays"), QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiScreenPage)},
        //~ contents_path /display/Brightness
        {"dcc_brightness", tr("Brightness"), QMetaMethod::fromSignal(&DisplayWidget::requestShowBrightnessPage)},
    };
    m_singleMenuList = {
        //~ contents_path /display/Resolution
        {"dcc_resolution", tr("Resolution"),QMetaMethod::fromSignal(&DisplayWidget::requestShowResolutionPage)},
        {"dcc_brightness", tr("Brightness"),QMetaMethod::fromSignal(&DisplayWidget::requestShowBrightnessPage)},
    };

    auto mons = m_model->monitorList();
    for (auto monitor : mons) {
       if (!monitor->canBrightness()) {
           mons.removeOne(monitor);
       }
    }

    bool brightnessEnable = m_displaySetting->get(GSETTINGS_BRIGHTNESS_ENABLE).toBool();

    if (mons.count() == 0 || !brightnessEnable) {
        if (getMenuIndex(tr("Brightness"), false) >= 0)
            m_multMenuList.removeAt(getMenuIndex(tr("Brightness"), false));
        if (getMenuIndex(tr("Brightness")) >= 0)
            m_singleMenuList.removeAt(getMenuIndex(tr("Brightness")));
    }

    if (!IsServerSystem) {
        // 平板下屏蔽屏幕缩放
        if (!DGuiApplicationHelper::isTabletEnvironment()) {
            //~ contents_path /display/Display Scaling
            ListSubItem scaleMenu = {"dcc_screen",tr("Display Scaling"),
                                    QMetaMethod::fromSignal(&DisplayWidget::requestShowScalingPage)};
            m_multMenuList << scaleMenu;
            m_singleMenuList << scaleMenu;
        }
    }

    //~ contents_path /display/Refresh Rate
    ListSubItem refreshMenu = { "dcc_refresh_rate",tr("Refresh Rate"),
                              QMetaMethod::fromSignal(&DisplayWidget::requestShowRefreshRatePage)
                             };
    m_singleMenuList << refreshMenu;

    if ((m_model && !m_model->touchscreenList().isEmpty()) && !IsServerSystem) {
        //~ contents_path /display/Touch Screen
        ListSubItem touchscreenMenu = {"dcc_touchscreen",tr("Touch Screen"),
                                      QMetaMethod::fromSignal(&DisplayWidget::requestShowTouchscreenPage)};

        // 平板一期触屏二级菜单隐藏
#ifndef USE_TABLET
        m_multMenuList << touchscreenMenu;
        m_singleMenuList << touchscreenMenu;
#endif
    }

    if (!m_isShowMultiscreen) {
        m_multMenuList.removeAt(0);
        ListSubItem multiRefreshMenu = {"dcc_refresh_rate",tr("Refresh Rate"), 
                                  QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiRefreshRatePage)
                                 };
        ListSubItem multiResoMenu = { "dcc_resolution",tr("Resolution"),
                                  QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiResolutionPage)
                                 };
        m_multMenuList << multiResoMenu << multiRefreshMenu;
        if (getMenuIndex(tr("Resolution")) >= 0)
            m_singleMenuList[getMenuIndex(tr("Resolution"))] = {"dcc_resolution",tr("Resolution"),  QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiResolutionPage)};
        if (getMenuIndex(tr("Refresh Rate")) >= 0)
            m_singleMenuList[getMenuIndex(tr("Refresh Rate"))] = { "dcc_resolution", tr("Refresh Rate"),QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiRefreshRatePage)};
    }

    DStandardItem *btn{nullptr};
    for (auto menu : m_multMenuList) {
        btn = new DStandardItem(menu.itemText);
        btn->setData(VListViewItemMargin, Dtk::MarginsRole);
        btn->setIcon(QIcon::fromTheme(menu.itemIcon));
        m_multiModel->appendRow(btn);
    }

    for (auto menu : m_singleMenuList) {
        btn = new DStandardItem(menu.itemText);
        btn->setData(VListViewItemMargin, Dtk::MarginsRole);
        btn->setIcon(QIcon::fromTheme(menu.itemIcon));
        m_singleModel->appendRow(btn);
    }

    if(InsertPlugin::instance()->needPushPlugin("display"))
        InsertPlugin::instance()->pushPlugin(m_singleModel,m_singleMenuList);

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
    m_rotate->setAccessibleName(tr("Rotate Screen"));

    m_centralLayout->addWidget(m_rotate, 0, Qt::AlignCenter);
    connect(m_rotate, &DFloatingButton::clicked, this, &DisplayWidget::requestRotate);
}

void DisplayWidget::onMenuClicked(const QModelIndex &idx)
{
    if (idx == m_currentIdx) return;

    m_currentIdx = idx;
    m_menuList->setCurrentIndex(m_currentIdx);
    if (m_isMultiScreen) {
        m_multMenuList[idx.row()].itemSignal.invoke(m_singleMenuList[idx.row()].pulgin ? m_singleMenuList[idx.row()].pulgin : this);
    } else {
        m_singleMenuList[idx.row()].itemSignal.invoke(m_singleMenuList[idx.row()].pulgin ? m_singleMenuList[idx.row()].pulgin : this);
    }
    m_menuList->resetStatus(idx);
}

void DisplayWidget::setDefaultWidget()
{
    if (m_isMultiScreen) {
        m_multMenuList[0].itemSignal.invoke(m_singleMenuList[0].pulgin ? m_singleMenuList[0].pulgin : this);
    } else {
        m_singleMenuList[0].itemSignal.invoke(m_singleMenuList[0].pulgin ? m_singleMenuList[0].pulgin : this);
    }
}

int DisplayWidget::getMenuIndex(QString str, bool isSingle)
{
    if (isSingle) {
        for (int i = 0; i < m_singleMenuList.count(); i++) {
            if (m_singleMenuList[i].itemText == str) {
                return i;
            }
        }
    } else {
        for (int i = 0; i < m_multMenuList.count(); i++) {
            if (m_multMenuList[i].itemText == str) {
                return i;
            }
        }
    }
    qDebug() << "no menu named " << str << " in " << (isSingle ? "single" : "multi") << " menu list or it has been already removed";
    return -1;
}
