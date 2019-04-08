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

#include "keyboardwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/searchinput.h"
#include "keyboardmodel.h"
#include <QDebug>

using namespace dcc;
using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

KeyboardWidget::KeyboardWidget(KeyboardModel *model)
    :ModuleWidget(),
      m_bDelay(false),
      m_model(model)
{
    setObjectName("Keyboard");

    this->installEventFilter(parent());
    setTitle(tr("Keyboard and Language"));

    SettingsGroup *keyTest = new SettingsGroup();
    TitledSliderItem* delayItem =  new TitledSliderItem(tr("Repeat Delay"));
    m_delaySlider = delayItem->slider();
    m_delaySlider->setType(DCCSlider::Vernier);
    m_delaySlider->setOrientation(Qt::Horizontal);
    m_delaySlider->setRange(1, 7);
    m_delaySlider->setTickInterval(1);
    m_delaySlider->setPageStep(1);
    m_delaySlider->setTickPosition(QSlider::TicksBelow);
    QStringList delays;
    delays<<tr("Short")<<""<<""<<""<<""<<""<<tr("Long");
    delayItem->setAnnotations(delays);
    keyTest->appendItem(delayItem);

    TitledSliderItem* speedItem =  new TitledSliderItem(tr("Repeat Rate"));
    m_speedSlider = speedItem->slider();
    m_speedSlider->setType(DCCSlider::Vernier);
    m_speedSlider->setOrientation(Qt::Horizontal);
    m_speedSlider->setRange(1, 7);
    m_speedSlider->setTickInterval(1);
    m_speedSlider->setPageStep(1);
    m_speedSlider->setTickPosition(QSlider::TicksBelow);
    QStringList speeds;
    speeds<<tr("Slow")<<""<<""<<""<<""<<""<<tr("Fast");
    speedItem->setAnnotations(speeds);

    SearchInput* testArea = new SearchInput();
    testArea->setFixedWidth(200);
    testArea->setSearchText(tr("Please Test Here"));
    testArea->setIconVisible(false);
    testArea->setStyleSheet("border: 0px");

    // adding extra stuff to speedItem
    QVBoxLayout *speedItemLayout = qobject_cast<QVBoxLayout*>(speedItem->layout());
    speedItemLayout->addWidget(testArea);
    speedItemLayout->setAlignment(testArea, Qt::AlignHCenter);
    speedItemLayout->addSpacing(10);
    speedItem->setFixedHeight(speedItemLayout->sizeHint().height());

    keyTest->appendItem(speedItem);

    m_numLock = new SwitchWidget;
    m_numLock->setTitle(tr("Enable Numeric Keyboard"));
    keyTest->appendItem(m_numLock);

    m_upper = new SwitchWidget();
    m_upper->setTitle(tr("Caps Lock Prompt"));
    keyTest->appendItem(m_upper);

#ifndef DCC_DISABLE_KBLAYOUT
    SettingsGroup* keyGroup = new SettingsGroup();
    m_keyItem = new NextPageWidget();
    m_keyItem->setTitle(tr("Keyboard Layout"));
    m_keyItem->setValue(tr(""));
    keyGroup->appendItem(m_keyItem);
#endif

#ifndef DCC_DISABLE_LANGUAGE
    SettingsGroup* langGroup = new SettingsGroup();
    m_langItem = new NextPageWidget();
    m_langItem->setTitle(tr("System Language"));
    langGroup->appendItem(m_langItem);
#endif

    SettingsGroup* scGroup = new SettingsGroup();
    m_scItem = new NextPageWidget();
    m_scItem->setTitle(tr("Shortcuts"));
    scGroup->appendItem(m_scItem);

    m_centralLayout->addWidget(keyTest);
#ifndef DCC_DISABLE_KBLAYOUT
    m_centralLayout->addWidget(keyGroup);
#endif
#ifndef DCC_DISABLE_LANGUAGE
    m_centralLayout->addWidget(langGroup);
#endif
    m_centralLayout->addWidget(scGroup);

#ifndef DCC_DISABLE_KBLAYOUT
    connect(m_keyItem,SIGNAL(clicked()), this, SIGNAL(keyoard()));
#endif
#ifndef DCC_DISABLE_LANGUAGE
    connect(m_langItem, SIGNAL(clicked()), this, SIGNAL(language()));
#endif
    connect(m_scItem, SIGNAL(clicked()), this, SIGNAL(shortcut()));
    connect(m_delaySlider, &DCCSlider::valueChanged, this, &KeyboardWidget::delayChanged);
    connect(m_speedSlider, &DCCSlider::valueChanged, this, &KeyboardWidget::speedChanged);
    connect(m_numLock, &SwitchWidget::checkedChanged, this, &KeyboardWidget::numLockChanged);
    connect(m_upper, SIGNAL(checkedChanged(bool)), this, SIGNAL(capsLockChanged(bool)));


    connect(m_model, &KeyboardModel::repeatDelayChanged, this, &KeyboardWidget::setDelayValue);
    connect(m_model, &KeyboardModel::repeatIntervalChanged, this, &KeyboardWidget::setSpeedValue);
    connect(m_model, &KeyboardModel::capsLockChanged,m_upper, &SwitchWidget::setChecked);
    connect(m_model, &KeyboardModel::numLockChanged, m_numLock, &SwitchWidget::setChecked);
#ifndef DCC_DISABLE_KBLAYOUT
    connect(m_model, &KeyboardModel::curLayoutChanged, m_keyItem, &NextPageWidget::setValue);
#endif
#ifndef DCC_DISABLE_LANGUAGE
    connect(m_model, &KeyboardModel::curLangChanged, m_langItem, &NextPageWidget::setValue);
#endif

    setDelayValue(m_model->repeatDelay());
    setSpeedValue(m_model->repeatInterval());
    m_upper->setChecked(m_model->capsLock());
    m_numLock->setChecked(m_model->numLock());
#ifndef DCC_DISABLE_KBLAYOUT
    m_keyItem->setValue(m_model->curLayout());
#endif
#ifndef DCC_DISABLE_LANGUAGE
    m_langItem->setValue(m_model->curLang());
#endif
}

void KeyboardWidget::setDelayValue(uint value)
{
    m_delaySlider->blockSignals(true);
    m_delaySlider->setValue(value);
    m_delaySlider->blockSignals(false);
}

void KeyboardWidget::setSpeedValue(uint value)
{
    m_speedSlider->blockSignals(true);
    m_speedSlider->setValue(value);
    m_speedSlider->blockSignals(false);
}

}
}
