/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "brightnesspage.h"
#include "displaymodel.h"
#include "monitor.h"
#include "settingsheaderitem.h"
#include "settingsgroup.h"
#include "brightnessitem.h"

using namespace dcc::widgets;

namespace dcc {

namespace display {

BrightnessPage::BrightnessPage(QWidget *parent)
    : ContentWidget(parent),

      m_centralLayout(new QVBoxLayout)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addSpacing(10);

    QWidget *w = new TranslucentFrame;
    w->setLayout(m_centralLayout);

    setTitle(tr("Brightness"));
    setContent(w);
}

void BrightnessPage::setModel(DisplayModel *model)
{
    m_displayModel = model;

    connect(model, &DisplayModel::monitorListChanged, this, &BrightnessPage::back);

    initUI();
}

void BrightnessPage::initUI()
{
    for (auto *mon : m_displayModel->monitorList())
    {
        BrightnessItem *slider = new BrightnessItem;
        connect(mon, &Monitor::brightnessChanged, slider, &BrightnessItem::setValue);
        connect(slider, &BrightnessItem::requestSetMonitorBrightness, this, &BrightnessPage::requestSetMonitorBrightness);
        slider->setMonitor(mon);
        slider->setValue(mon->brightness());

        SettingsGroup *grp = new SettingsGroup;
        grp->appendItem(slider);
        grp->setHeaderVisible(true);
        grp->headerItem()->setTitle(mon->name());

        m_centralLayout->addWidget(grp);
    }
}

}

}
