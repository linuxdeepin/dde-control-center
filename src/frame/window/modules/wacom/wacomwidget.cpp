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

#include "window/modules/wacom/wacommodel.h"
#include "window/modules/wacom/pressuresettings.h"

#include "widgets/comboxwidget.h"

#include <QComboBox>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::wacom;

WacomWidget::WacomWidget(QWidget *parent)
    : QWidget(parent)
    , m_sensitivity(new PressureSettings)
    , m_centralLayout(new QVBoxLayout)
    , m_selectWacomMode(new ComboxWidget)
{
    m_selectWacomMode->setTitle(tr("Mode"));
    m_selectWacomMode->comboBox()->addItem(getModeName(true)); // 笔模式
    m_selectWacomMode->comboBox()->addItem(getModeName(false));

    m_selectWacomMode->comboBox()->setCurrentIndex(0);

    m_centralLayout->addWidget(m_selectWacomMode);
    m_centralLayout->addWidget(m_sensitivity);

    setLayout(m_centralLayout);

    connect(m_sensitivity, &PressureSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
    connect(m_selectWacomMode, &ComboxWidget::onSelectChanged, this, [ = ](const QString curMode) {
        bool isPen = false;
        if (tr("Pen") == curMode) {
            isPen = true;
        }
        Q_EMIT WacomWidget::onCursorModeChanged(isPen);
    } );
}

void WacomWidget::setModel(WacomModel *model)
{
    m_sensitivity->setModel(model);

    connect(model, &WacomModel::cursorModeChanged, this, &WacomWidget::onCursorModeChanged);
    onCursorModeChanged(model->getCursorMode());
}

QString WacomWidget::getModeName(const bool curMode) const
{
    if (curMode) {
        return tr("Pen");
    } else {
        return tr("Mouse");
    }
}

void WacomWidget::onCursorModeChanged(const bool curMode)
{
    int index = 1;
    if (curMode) {
        index = 0;
    }
    m_sensitivity->setVisible(curMode);
    m_selectWacomMode->comboBox()->setCurrentIndex(index);
    Q_EMIT modeChanged(curMode);
}
