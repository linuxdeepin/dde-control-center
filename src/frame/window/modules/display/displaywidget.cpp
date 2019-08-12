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
#include "window/namespace.h"

#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"

#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

DisplayWidget::DisplayWidget(QWidget *parent)
    : QWidget(parent)
    , m_rotate(new DImageButton(this))
    , m_centralLayout(new QVBoxLayout(this))
    , m_menuList(new QListView(this))
    , m_multiModel(new QStandardItemModel(this))
    , m_singleModel(new QStandardItemModel(this))
{
    setObjectName("Display");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMaximumWidth(500);
    setMinimumWidth(300);

    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addSpacing(10);
    setLayout(m_centralLayout);

    initMenuUI();
}

void DisplayWidget::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configListChanged, this, &DisplayWidget::onMonitorListChanged);
    connect(m_model, &DisplayModel::configCreated, this, &DisplayWidget::requestShowCustomConfigPage);

    onMonitorListChanged();
}

void DisplayWidget::onMonitorListChanged()
{
    const auto mons = m_model->monitorList();

    if (mons.size() <= 1) {
        m_isMultiScreen = false;
        m_menuList->setModel(m_singleModel);
        m_model->setAllowEnableMultiScaleRatio(false);
    } else {
        m_isMultiScreen = true;
        m_menuList->setModel(m_multiModel);
        m_model->setAllowEnableMultiScaleRatio(true);
    }
}

void DisplayWidget::initMenuUI()
{
    m_multMenuList = {
        {tr("Mutil-Screen"), QMetaMethod::fromSignal(&DisplayWidget::requestShowMultiScreenPage)},
        {tr("Resolution"), QMetaMethod::fromSignal(&DisplayWidget::requestShowResolutionPage)},
        {tr("Brightness"), QMetaMethod::fromSignal(&DisplayWidget::requestShowBrightnessPage)},
        {tr("scaling"), QMetaMethod::fromSignal(&DisplayWidget::requestShowScalingPage)}
    };

    m_singleMenuList = {
        {tr("Resolution"), QMetaMethod::fromSignal(&DisplayWidget::requestShowResolutionPage)},
        {tr("Brightness"), QMetaMethod::fromSignal(&DisplayWidget::requestShowBrightnessPage)},
        {tr("scaling"), QMetaMethod::fromSignal(&DisplayWidget::requestShowScalingPage)}
    };

    QStandardItem *btn{nullptr};
    for (auto menu : m_multMenuList) {
        btn = new QStandardItem(menu.menuText);
        m_multiModel->appendRow(btn);
    }

    for (auto menu : m_singleMenuList) {
        btn = new QStandardItem(menu.menuText);
        m_singleModel->appendRow(btn);
    }

    m_centralLayout->addWidget(m_menuList);
    connect(m_menuList, &QListView::clicked, this, &DisplayWidget::onMenuClicked);

    m_centralLayout->addStretch(1);
    m_rotate->setNormalPic(":/display/themes/dark/icons/rotate_normal.png");
    m_rotate->setHoverPic(":/display/themes/dark/icons/rotate_hover.png");
    m_rotate->setPressPic(":/display/themes/dark/icons/rotate_press.png");

    m_centralLayout->addWidget(m_rotate);
    connect(m_rotate, &DImageButton::clicked, this, &DisplayWidget::requestRotate);
}

void DisplayWidget::onMenuClicked(const QModelIndex &idx)
{
    if (m_isMultiScreen)
        m_multMenuList[idx.row()].method.invoke(this);
    else
        m_singleMenuList[idx.row()].method.invoke(this);
}

int DisplayWidget::convertToSlider(const float value)
{
    //remove base scale (100), then convert to 1-based value
    //with a stepping of 25
    return int(round(double(value * 100 - 100) / 25)) + 1;
}

float DisplayWidget::convertToScale(const int value)
{
    return float(1.0 + (value - 1) * 0.25);
}
