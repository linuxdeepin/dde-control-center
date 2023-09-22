//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "wacommodule.h"
#include "widgets/widgetmodule.h"
#include "widgets/itemmodule.h"
#include <settingsgroupmodule.h>
#include <titledslideritem.h>
#include <wacommodel.h>
#include <widgetmodule.h>
#include "widgets/dccslider.h"

#include <DSlider>
#include <DIconTheme>

#include <QComboBox>
#include <QLoggingCategory>
#include <QDebug>

Q_LOGGING_CATEGORY(DdcWacomModule, "dcc-wacom-module")

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

WacomModule::WacomModule(QObject *parent)
    : PageModule("wacom", tr("Drawing Tablet") , tr("Drawing Tablet"), DIconTheme::findQIcon("dcc_nav_wacom"), parent)
    , m_model(new WacomModel(this))
{
    connect(m_model, &WacomModel::ExistChanged, this, [this](bool exist){
        this->setHidden(!exist);
        qCInfo(DdcWacomModule) << "Wacom is exist ?:" << m_model->exist();
    });
    // Mode
    appendChild(new ItemModule("Mode", tr("Mode"), this, &WacomModule::initModeModule,true));
    // Pressure
    appendChild(new ItemModule("Pressure",tr("Pressure Sensitivity"),this, &WacomModule::initPressureModule,false));
    setHidden(!m_model->exist());
    qCInfo(DdcWacomModule) << "Wacom is exist ?:" << m_model->exist();
}

QWidget* WacomModule::initModeModule(ModuleObject *module)
{
    Q_UNUSED(module);

    QComboBox *modeComboBox = new QComboBox;
    auto onCursorModeChanged = [modeComboBox](const bool curMode) -> void {
        modeComboBox->blockSignals(true);
        modeComboBox->setCurrentIndex(curMode ? 1 : 0);
        modeComboBox->blockSignals(false);
    };

    connect(m_model, &WacomModel::CursorModeChanged, modeComboBox, [onCursorModeChanged](const bool curMode){
        onCursorModeChanged(curMode);
    });
    connect(modeComboBox, &QComboBox::currentTextChanged, this, [ = ](const QString curMode) {
        Q_UNUSED(curMode);
        QVariant curData = modeComboBox->currentData();
        m_model->setCursorMode(curData.toBool());
    });

    modeComboBox->addItem(tr("Pen"), false);
    modeComboBox->addItem(tr("Mouse"), true);
    modeComboBox->setCurrentIndex(0);

    onCursorModeChanged(m_model->CursorMode());
    return modeComboBox;
}

QWidget *WacomModule::initPressureModule(ModuleObject *module)
{
    Q_UNUSED(module);
    SettingsGroup *group = new SettingsGroup;

    connect(m_model, &WacomModel::CursorModeChanged, group, [group](const bool curMode){
        group->setVisible(!curMode);
    });

    DCC_NAMESPACE::TitledSliderItem *pressureSlider = new DCC_NAMESPACE::TitledSliderItem(tr("Pressure Sensitivity"));
    pressureSlider->slider()->setType(DCCSlider::Vernier);
    pressureSlider->slider()->setTickPosition(QSlider::TicksBelow);
    pressureSlider->slider()->setRange(1, 7);
    pressureSlider->slider()->setTickInterval(1);
    pressureSlider->slider()->setPageStep(1);

    QStringList delays;
    delays<<tr("Light")<<""<<""<<""<<""<<""<<tr("Heavy");
    pressureSlider->setAnnotations(delays);
    group->appendItem(pressureSlider);

    DSlider * preSlider = qobject_cast<DSlider *>(pressureSlider->slider());
    connect(m_model, &WacomModel::EraserPressureSensitiveChanged, preSlider, &DSlider::setValue);
    connect(preSlider, &DSlider::valueChanged, m_model, &WacomModel::setEraserPressureSensitive);
    preSlider->setValue(static_cast<int>(m_model->eraserPressureSensitive()));
    group->setVisible(!m_model->CursorMode());
    return group;
}
