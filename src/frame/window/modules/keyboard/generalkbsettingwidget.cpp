/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "generalkbsettingwidget.h"
#include "window/utils.h"
#include "widgets/dccslider.h"
#include "widgets/settingsgroup.h"
#include "widgets/titledslideritem.h"
#include "widgets/switchwidget.h"
#include "modules/keyboard/keyboardmodel.h"

#include <DApplicationHelper>

#include <QVBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <DStyle>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;
using namespace dcc::widgets;

GeneralKBSettingWidget::GeneralKBSettingWidget(KeyboardModel *model, QWidget *parent)
    : dcc::ContentWidget(parent)
    , m_model(model)
{
    m_generalSettingsGrp = new SettingsGroup();
    //~ contents_path /keyboard/General
    TitledSliderItem *delayItem =  new TitledSliderItem(tr("Repeat Delay"));
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
    //~ contents_path /keyboard/General
    TitledSliderItem *speedItem =  new TitledSliderItem(tr("Repeat Rate"));
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
    //~ contents_path /keyboard/General
    m_testArea->lineEdit()->setPlaceholderText(tr("Test here"));
    m_testArea->lineEdit()->setAlignment(Qt::AlignCenter);
    m_testArea->setClearButtonEnabled(false);
    DStyle::setFocusRectVisible(m_testArea->lineEdit(), false);

    DApplicationHelper *testAreaHelper = DApplicationHelper::instance();
    auto pa = testAreaHelper->palette(m_testArea);
    pa.setColor(DPalette::Button, Qt::transparent);
    testAreaHelper->setPalette(m_testArea, pa);

    // adding extra stuff to speedItem
    QVBoxLayout *speedItemLayout = qobject_cast<QVBoxLayout *>(speedItem->layout());
    speedItemLayout->addSpacing(List_Interval);
    speedItemLayout->addWidget(m_testArea);
    speedItemLayout->setAlignment(m_testArea, Qt::AlignHCenter);
    speedItemLayout->addSpacing(List_Interval);
    speedItem->setFixedHeight(speedItemLayout->sizeHint().height());

    m_generalSettingsGrp->appendItem(speedItem);

    m_numLock = new SwitchWidget;
    //~ contents_path /keyboard/General
    m_numLock->setTitle(tr("Numeric Keypad"));
    m_generalSettingsGrp->appendItem(m_numLock);

    m_upper = new SwitchWidget();
    //~ contents_path /keyboard/General
    m_upper->setTitle(tr("Caps Lock Prompt"));
    m_generalSettingsGrp->appendItem(m_upper);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setMargin(0);
    m_contentLayout->addWidget(m_generalSettingsGrp);
    m_contentLayout->addStretch();

    TranslucentFrame *tFrame = new TranslucentFrame;
    tFrame->setLayout(m_contentLayout);
    layout()->setMargin(0);
    setContent(tFrame);

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

