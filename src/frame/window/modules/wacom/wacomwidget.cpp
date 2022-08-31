// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    setAccessibleName("WacomWidget");
    m_modeLayout->setMargin(0);
    m_modeLayout->setSpacing(0);

    //~ contents_path /wacom/Mode
    m_modeTitle->setText(tr("Mode"));
    m_modeTitle->setAccessibleName("WacomWidget_modeTitle");

    m_modeComboBox->setAccessibleName("WacomWidget_modeComboBox");
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
    m_sensitivity->setVisible(!curMode);
    m_modeComboBox->blockSignals(true);
    m_modeComboBox->setCurrentIndex(index);
    m_modeComboBox->blockSignals(false);
}
