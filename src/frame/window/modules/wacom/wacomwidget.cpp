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
#include "window/utils.h"

#include "widgets/settingsitem.h"

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::wacom;

WacomWidget::WacomWidget(QWidget *parent)
    : QWidget(parent)
    , m_sensitivity(new PressureSettings)
    , m_centralLayout(new QVBoxLayout)
    , m_modeLayout(new QHBoxLayout)
    , m_modeTitle(new QLabel)
    , m_modeComboBox(new QComboBox)
    , m_modeSetting(new SettingsItem)
{
    initWidget();

    connect(m_sensitivity, &PressureSettings::requestSetPressureValue, this, &WacomWidget::requestSetPressureValue);
    connect(m_modeComboBox, &QComboBox::currentTextChanged, this, [ = ](const QString curMode) {
        Q_UNUSED(curMode);
        QVariant curData = m_modeComboBox->currentData();
        Q_EMIT WacomWidget::modeChanged(curData.toBool());
    } );
}

void WacomWidget::setModel(WacomModel *model)
{
    m_sensitivity->setModel(model);

    connect(model, &WacomModel::cursorModeChanged, this, &WacomWidget::onCursorModeChanged);
    onCursorModeChanged(model->getCursorMode());
}

void WacomWidget::initWidget()
{
    m_modeLayout->setMargin(0);
    m_modeLayout->setSpacing(0);

    m_modeTitle->setText(tr("Mode"));

    m_modeComboBox->addItem(tr("Pen"), false);
    m_modeComboBox->addItem(tr("Mouse"), true);
    m_modeComboBox->setCurrentIndex(0);

    m_modeLayout->setContentsMargins(10, 6, 10, 6);
    m_modeLayout->addWidget(m_modeTitle, 1);
    m_modeLayout->addWidget(m_modeComboBox, 1);

    m_modeSetting->setLayout(m_modeLayout);
    m_modeSetting->addBackground();

    m_centralLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addWidget(m_modeSetting);
    m_centralLayout->addWidget(m_sensitivity);
    m_centralLayout->setMargin(0);
    m_centralLayout->setContentsMargins(ThirdPageContentsMargins);

    setLayout(m_centralLayout);
}

void WacomWidget::onCursorModeChanged(const bool curMode)
{
    int index = 0;
    if (curMode) {
        index = 1;
    }
    m_sensitivity->setVisible(curMode);
    m_modeComboBox->blockSignals(true);
    m_modeComboBox->setCurrentIndex(index);
    m_modeComboBox->blockSignals(false);
}
