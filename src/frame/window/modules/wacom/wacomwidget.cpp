/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wp <wangpeng_cm@deepin.com>
 *
 * Maintainer: wp <wangpeng_cm@deepin.com>
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

#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "modules/wacom/wacommodel.h"
#include "modules/wacom/widget/wacomsettings.h"

#include <QPushButton>

using namespace dcc::wacom;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::wacom;
WacomWidget::WacomWidget(QWidget *parent)
    : QWidget(parent)
    , m_selectMode(new NextPageWidget())
    , m_centralLayout(new QVBoxLayout)
{
    setObjectName("Wacom");
    setLayout(m_centralLayout);

    m_Sensitivity = new WacomSettings;
    SettingsGroup *modeGrp = new SettingsGroup;
    modeGrp->appendItem(m_selectMode);
    m_selectMode->setTitle(tr("Mode"));

    m_centralLayout->addWidget(modeGrp);
    m_centralLayout->addWidget(m_Sensitivity);

    connect(m_Sensitivity, &WacomSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
    connect(m_selectMode, &NextPageWidget::clicked, this, &WacomWidget::requestShowMode);
}

void WacomWidget::setModel(WacomModel *const model)
{
    m_Sensitivity->setModel(model->getWacomModelBase());

    connect(model, &WacomModel::cursorModeChanged, this, &WacomWidget::onCursorModeChanged);
    onCursorModeChanged(model->getCursorMode());
}

QString WacomWidget::getValue(const Mode mode) const
{
    QString strmode;
    switch (mode) {
    case Mode::MOUSE:
        strmode = tr("Mouse");
        break;
    case Mode::PEN:
        strmode = tr("Pen");
        break;
    }

    return strmode;
}

void WacomWidget::onCursorModeChanged(const bool modeChanged)
{
    if (modeChanged)
        m_selectMode->setValue(getValue(Mode::MOUSE));
    else
        m_selectMode->setValue(getValue(Mode::PEN));

    m_Sensitivity->setVisible(!modeChanged);
}
