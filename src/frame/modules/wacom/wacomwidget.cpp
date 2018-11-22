/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "wacomwidget.h"

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "wacommodel.h"
#include "widget/wacomsettings.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::wacom;
WacomWidget::WacomWidget()
    : ModuleWidget(),
      m_selectMode(new NextPageWidget())
{
    setObjectName("Wacom");

    SettingsGroup *modeGrp = new SettingsGroup;
    modeGrp->appendItem(m_selectMode);
    m_selectMode->setTitle(tr("Mode"));

    m_centralLayout->addWidget(modeGrp);

    m_wacomSettings = new WacomSettings;
    m_centralLayout->addWidget(m_wacomSettings);

    setTitle(tr("Wacom"));
    connect(m_wacomSettings, &WacomSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
    connect(m_selectMode, &NextPageWidget::clicked, this, &WacomWidget::requestShowMode);
}

void WacomWidget::setModel(WacomModel *const model)
{
    m_wacomSettings->setModel(model->getWacomModelBase());

    connect(model, &WacomModel::cursorModeChanged, this, &WacomWidget::onCursorModeChanged);
    onCursorModeChanged(model->getCursorMode());
}

const QString WacomWidget::getValue(const bool mode)
{
    return mode ? tr("Mouse") : tr("Pen");
}

void WacomWidget::onCursorModeChanged(const bool modeChanged)
{
    m_selectMode->setValue(getValue(modeChanged));
    m_wacomSettings->setVisible(!modeChanged);
}
