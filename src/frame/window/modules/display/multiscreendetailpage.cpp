/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     dr <dongrui@deepin.com>
 *
 * Maintainer: dr <dongrui@deepin.com>
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

#include "multiscreendetailpage.h"
#include "modules/display/displaymodel.h"
#include "window/utils.h"

#include <QPushButton>

using namespace DCC_NAMESPACE::display;
using namespace dcc::widgets;
using namespace dcc::display;

MultiScreenDetailPage::MultiScreenDetailPage(bool type, QWidget *parent)
    : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(ThirdPageContentsMargins);

    m_refreshOrResolution = type;

    setLayout(m_mainLayout);
}

void MultiScreenDetailPage::setModel(dcc::display::DisplayModel *model)
{
    m_model = model;

    initUI();
}

void MultiScreenDetailPage::initUI()
{
    auto monitors = m_model->monitorList();
    for(int i = 0; i < monitors.size(); ++i) {
        if (!monitors[i]->enable())
            return;
        TitleValueItem *title = new TitleValueItem;
        Monitor *moni = monitors[i];
        title->setTitle(moni->name());
        QString resolution = QString("%1x%2").arg(moni->currentMode().width()).arg(moni->currentMode().height());
        QString rate = QString::number(moni->currentMode().rate(), 10, 2);
        title->setValue(m_refreshOrResolution ? resolution : rate);
        m_mainLayout->addWidget(title, 0, Qt::AlignTop);
    }
    m_mainLayout->addStretch();
}

