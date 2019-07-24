/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
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
#include "touchpadsettingwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "../../../modules/mouse/widget/palmdetectsetting.h"
#include "widgets/dccslider.h"
#include "../../../modules/mouse/widget/doutestwidget.h"
#include "../../../modules/mouse/mousemodel.h"
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace dcc::mouse;
using namespace dcc::widgets;

TouchPadSettingWidget::TouchPadSettingWidget(QWidget *parent) : ContentWidget(parent)
{
    m_touchpadSettingsGrp = new SettingsGroup;

    m_touchMoveSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_touchClickStn = new SwitchWidget(tr("Tap to Click"));
    m_touchNaturalScroll = new SwitchWidget(tr("Natural Scrolling"));

    m_touchpadSettingsGrp->appendItem(m_touchMoveSlider);
    m_touchpadSettingsGrp->appendItem(m_touchClickStn);
    m_touchpadSettingsGrp->appendItem(m_touchNaturalScroll);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->addWidget(m_touchpadSettingsGrp);
    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(m_contentLayout);
    setContent(w);
}

void TouchPadSettingWidget::setModel(MouseModel *const model)
{
    m_mouseModel = model;
}
