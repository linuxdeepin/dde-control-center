//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "generalkbsettingwidget.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"
#include "widgets/titledslideritem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingshead.h"
#include "widgets/settingsheaderitem.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QLineEdit>


#define GSETTINGS_NUMLOCK_ENABLE "keyboardGeneralNumlockEnable"
using namespace DCC_NAMESPACE;

GeneralKBSettingWidget::GeneralKBSettingWidget(KeyboardModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
{
    SettingsHead *systemHead = new SettingsHead();
    systemHead->setEditEnable(false);
    systemHead->setTitle(tr("Keyboard Settings"));
    systemHead->layout()->setContentsMargins(10, 0, 10, 0);
    m_generalSettingsGrp = new SettingsGroup();
    m_generalSettingsGrp->appendItem(systemHead, SettingsGroup::NoneBackground);
    TitledSliderItem *delayItem =  new TitledSliderItem(tr("Repeat Delay"));
    delayItem->setObjectName("RepeatDelay");
    m_delaySlider = delayItem->slider();
    m_delaySlider->setType(DCCSlider::Vernier);
    m_delaySlider->setOrientation(Qt::Horizontal);
    m_delaySlider->setRange(1, 7);
    m_delaySlider->setTickInterval(1);
    m_delaySlider->setPageStep(1);
    m_delaySlider->setTickPosition(QSlider::TicksBelow);
    QStringList delays;
    delays << tr("Short") << "" << "" << "" << "" << "";
    delays << tr("Long");
    delayItem->setAnnotations(delays);
    m_generalSettingsGrp->appendItem(delayItem);
    TitledSliderItem *speedItem =  new TitledSliderItem(tr("Repeat Rate"));
    speedItem->setObjectName("RepeatRate");
    speedItem->setFocusPolicy(Qt::ClickFocus);
    m_speedSlider = speedItem->slider();
    m_speedSlider->setType(DCCSlider::Vernier);
    m_speedSlider->setOrientation(Qt::Horizontal);
    m_speedSlider->setRange(1, 7);
    m_speedSlider->setTickInterval(1);
    m_speedSlider->setPageStep(1);
    m_speedSlider->setTickPosition(QSlider::TicksBelow);
    QStringList speeds;
    speeds << tr("Slow") << "" << "" << "" << "" << "";
    speeds << tr("Fast");
    speedItem->setAnnotations(speeds);

    m_testArea = new DLineEdit();
    m_testArea->setFixedWidth(200);
    m_testArea->lineEdit()->setPlaceholderText(tr("Test here"));
    m_testArea->lineEdit()->setAlignment(Qt::AlignCenter);
    m_testArea->setClearButtonEnabled(false);
    DStyle::setFocusRectVisible(m_testArea->lineEdit(), false);

    auto pa = DPaletteHelper::instance()->palette(m_testArea);
    pa.setColor(DPalette::Button, Qt::transparent);
    DPaletteHelper::instance()->setPalette(m_testArea, pa);

    // adding extra stuff to speedItem
    QVBoxLayout *speedItemLayout = qobject_cast<QVBoxLayout *>(speedItem->layout());
    speedItemLayout->addSpacing(10);
    speedItemLayout->addWidget(m_testArea);
    speedItemLayout->setAlignment(m_testArea, Qt::AlignHCenter);
    speedItemLayout->addSpacing(10);
    speedItem->setFixedHeight(speedItemLayout->sizeHint().height());

    m_generalSettingsGrp->appendItem(speedItem);

    m_numLock = new SwitchWidget;
    m_numLock->setTitle(tr("Numeric Keypad"));
    m_numLock->setObjectName("NumLock");
    m_generalSettingsGrp->appendItem(m_numLock);

    m_upper = new SwitchWidget();
    m_upper->setTitle(tr("Caps Lock Prompt"));
    m_upper->setObjectName("Upper");
    m_generalSettingsGrp->appendItem(m_upper);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setMargin(0);
    m_contentLayout->addWidget(m_generalSettingsGrp);
    m_contentLayout->addStretch();

    setLayout(m_contentLayout);
    setContentsMargins(0, 10, 0, 10);

    connect(m_delaySlider, &DCCSlider::valueChanged, this, &GeneralKBSettingWidget::requestKBDelayChanged);
    connect(m_speedSlider, &DCCSlider::valueChanged, this, &GeneralKBSettingWidget::requestKBSpeedChanged);
    connect(m_numLock, &SwitchWidget::checkedChanged, this, &GeneralKBSettingWidget::requestNumLockChanged);
    connect(m_upper, &SwitchWidget::checkedChanged, this, &GeneralKBSettingWidget::requestCapsLockChanged);

    connect(m_model, &KeyboardModel::repeatDelayChanged, this, &GeneralKBSettingWidget::setDelayValue);
    connect(m_model, &KeyboardModel::repeatIntervalChanged, this, &GeneralKBSettingWidget::setSpeedValue);
    connect(m_model, &KeyboardModel::capsLockChanged, m_upper, &SwitchWidget::setChecked);
    connect(m_model, &KeyboardModel::numLockChanged, m_numLock, &SwitchWidget::setChecked);
    connect(m_testArea, &DLineEdit::focusChanged, this, [ = ] {
        m_testArea->clear();
        m_testArea->update();
    });

    setDelayValue(m_model->repeatDelay());
    setSpeedValue(m_model->repeatInterval());
    m_upper->setChecked(m_model->capsLock());
    m_numLock->setChecked(m_model->numLock());
}

void GeneralKBSettingWidget::setDelayValue(uint value)
{
    m_delaySlider->blockSignals(true);
    m_delaySlider->setValue(static_cast<int>(value));
    m_delaySlider->blockSignals(false);
}

void GeneralKBSettingWidget::setSpeedValue(uint value)
{
    m_speedSlider->blockSignals(true);
    m_speedSlider->setValue(static_cast<int>(value));
    m_speedSlider->blockSignals(false);
}
