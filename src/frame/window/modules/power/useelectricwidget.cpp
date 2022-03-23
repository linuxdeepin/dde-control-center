/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "useelectricwidget.h"

#include <QPushButton>
#include <QtMath>
#include <QComboBox>
#include <QGSettings>

#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"
#include "widgets/optionitem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/power/powermodel.h"
#include "modules/power/powerworker.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/comboxwidget.h"
#include "widgets/contentwidget.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

UseElectricWidget::UseElectricWidget(PowerModel *model, QWidget *parent, dcc::power::PowerWorker *work)
    : QWidget(parent)
    , m_model(model)
    , m_work(work)
    , m_layout(new QVBoxLayout)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_autoLockScreen = new TitledSliderItem(tr("Lock screen after"));
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_cmbPowerBtn = new ComboxWidget(tr("When pressing the power button"));
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_cmbCloseLid = new ComboxWidget(tr("When the lid is closed"));
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_autoLockScreen->setAccessibleName(tr("Lock screen after"));

    SettingsGroup *powerSettingsGrp = new SettingsGroup;
    powerSettingsGrp->setSpacing(List_Interval);

    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_monitorSleepOnPower = new TitledSliderItem(tr("Monitor will suspend after"));
    m_monitorSleepOnPower->setAccessibleName(tr("Monitor will suspend after"));
    m_monitorSleepOnPower->slider()->setType(DCCSlider::Vernier);
    m_monitorSleepOnPower->slider()->setRange(1, 7);
    m_monitorSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleepOnPower->slider()->setTickInterval(1);
    m_monitorSleepOnPower->slider()->setPageStep(1);
    connect(m_monitorSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetScreenBlackDelayOnPower);
    powerSettingsGrp->appendItem(m_monitorSleepOnPower);

    if (!IsServerSystem) {
        //~ contents_path /power/Plugged In
        //~ child_page Plugged In
        m_computerSleepOnPower = new TitledSliderItem(tr("Computer will suspend after"));
        m_computerSleepOnPower->setAccessibleName(tr("Computer will suspend after"));
        m_computerSleepOnPower->slider()->setType(DCCSlider::Vernier);
        m_computerSleepOnPower->slider()->setRange(1, 7);
        m_computerSleepOnPower->slider()->setTickPosition(QSlider::TicksBelow);
        m_computerSleepOnPower->slider()->setTickInterval(1);
        m_computerSleepOnPower->slider()->setPageStep(1);
        connect(m_computerSleepOnPower->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetSleepDelayOnPower);
        powerSettingsGrp->appendItem(m_computerSleepOnPower);
    }

    powerSettingsGrp->appendItem(m_autoLockScreen);
    powerSettingsGrp->appendItem(m_cmbCloseLid);
    powerSettingsGrp->appendItem(m_cmbPowerBtn);
    // 使用GSettings来控制显示状态
    GSettingWatcher::instance()->bind("powerLidPresent", m_cmbCloseLid);
    GSettingWatcher::instance()->bind("powerPressPowerbtn", m_cmbPowerBtn);
    GSettingWatcher::instance()->bind("powerAutoLockscreen", m_autoLockScreen);
    GSettingWatcher::instance()->bind("powerMonitorConfigure", m_monitorSleepOnPower);
    GSettingWatcher::instance()->bind("systemSuspend", m_computerSleepOnPower);

    m_layout->setContentsMargins(10, 10, 2, 5);
    m_layout->addWidget(powerSettingsGrp);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(10);

    ContentWidget *contentWgt = new ContentWidget;
    contentWgt->setAccessibleName("UseElectricWidget_Content");
    QWidget *mainWgt = new TranslucentFrame;
    mainWgt->setLayout(m_layout);
    contentWgt->setContent(mainWgt);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(contentWgt);
    mainLayout->setContentsMargins(0, 0, 8, 8);
    setLayout(mainLayout);

    QStringList annos;
    annos << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");
    if (!IsServerSystem) {
        m_computerSleepOnPower->setAnnotations(annos);
    }
    m_monitorSleepOnPower->setAnnotations(annos);

    m_autoLockScreen->slider()->setType(DCCSlider::Vernier);
    m_autoLockScreen->slider()->setRange(1, 7);
    m_autoLockScreen->slider()->setTickPosition(QSlider::TicksBelow);
    m_autoLockScreen->slider()->setTickInterval(1);
    m_autoLockScreen->slider()->setPageStep(1);
    m_autoLockScreen->setAnnotations(annos);

    setModel(model);

    connect(m_autoLockScreen->slider(), &DCCSlider::valueChanged, this, &UseElectricWidget::requestSetAutoLockScreenOnPower);

    connect(m_cmbPowerBtn->comboBox(), &AlertComboBox::clicked, this, [ = ]() {
        updatePowerButtonActionList();
        setPowerBtn(model, model->linePowerPressPowerBtnAction());
        setCloseLid(model, model->linePowerLidClosedAction());
    });


    connect(m_cmbPowerBtn, &ComboxWidget::onIndexChanged, this, [ = ](int nIndex) {
        if (nIndex < 0) return;

        int tmpIndex = nIndex;
        if (!model->getShutdown()) {
            tmpIndex += 1;
        }

        if (!model->getSuspend() && tmpIndex >= 1) {
            tmpIndex += 1;
        }

        if (!model->getHibernate() && tmpIndex >= 2) {
            tmpIndex += 1;
        }

        Q_EMIT requestSetLinePowerPressPowerBtnAction(tmpIndex);
    });

    connect(m_cmbCloseLid->comboBox(), &AlertComboBox::clicked, this, [ = ]() {
        updatePowerButtonActionList();
        setPowerBtn(model, model->linePowerPressPowerBtnAction());
        setCloseLid(model, model->linePowerLidClosedAction());
    });
    connect(m_cmbCloseLid, &ComboxWidget::onIndexChanged, [ = ](int nIndex) {
        if (nIndex < 0) return;

        int tmpIndex = nIndex;
        if (!model->getShutdown() && tmpIndex >= 0) {
            tmpIndex += 1;
        }

        if (!model->getSuspend() && tmpIndex >= 1) {
            tmpIndex += 1;
        }

        if (!model->getHibernate() && tmpIndex >= 2) {
            tmpIndex += 1;
        }

        Q_EMIT requestSetLinePowerLidClosedAction(tmpIndex);
    });

}

UseElectricWidget::~UseElectricWidget()
{
    GSettingWatcher::instance()->erase("powerLidPresent", m_cmbCloseLid);
    GSettingWatcher::instance()->erase("powerPressPowerbtn", m_cmbPowerBtn);
    GSettingWatcher::instance()->erase("powerAutoLockscreen", m_autoLockScreen);
    GSettingWatcher::instance()->erase("powerMonitorConfigure", m_monitorSleepOnPower);
    GSettingWatcher::instance()->erase("systemSuspend", m_computerSleepOnPower);
}

void UseElectricWidget::setModel(const PowerModel *model)
{
    connect(model, &PowerModel::powerLockScreenDelayChanged, this, &UseElectricWidget::setLockScreenAfter);
    connect(model, &PowerModel::hibernateChanged, this, [ = ] {
        updatePowerButtonActionList();
        setPowerBtn(model, model->linePowerPressPowerBtnAction());
        setCloseLid(model, model->linePowerLidClosedAction());
    });
    connect(model, &PowerModel::suspendChanged, this, [ = ] {
        updatePowerButtonActionList();
        setPowerBtn(model, model->linePowerPressPowerBtnAction());
        setCloseLid(model, model->linePowerLidClosedAction());
    });

    connect(model, &PowerModel::shutdownChanged, this, [ = ] {
        updatePowerButtonActionList();
        setPowerBtn(model, model->linePowerPressPowerBtnAction());
        setCloseLid(model, model->linePowerLidClosedAction());
    });

    if (!IsServerSystem) {
        connect(model, &PowerModel::sleepDelayChangedOnPower, this, &UseElectricWidget::setSleepDelayOnPower);
        setSleepDelayOnPower(model->sleepDelayOnPower());
    }

    setLockScreenAfter(model->getPowerLockScreenDelay());

    if (m_monitorSleepOnPower) {
        connect(model, &PowerModel::screenBlackDelayChangedOnPower, this, &UseElectricWidget::setScreenBlackDelayOnPower);
        setScreenBlackDelayOnPower(model->screenBlackDelayOnPower());
    }

    if (m_computerSleepOnPower) {
        m_computerSleepOnPower->setVisible(model->canSuspend() && model->getSuspend()
                                                             && (GSettingWatcher::instance()->getStatus("systemSuspend") != "Hidden"));
    }

    //--------------sp2 add-----------------
    connect(model, &PowerModel::lidPresentChanged, this, [ = ](bool value) {
        m_cmbCloseLid->setVisible(value && GSettingWatcher::instance()->getStatus("powerLidPresent") != "Hidden");
    });
    m_cmbCloseLid->setVisible(model->lidPresent() && GSettingWatcher::instance()->getStatus("powerLidPresent") != "Hidden");
    connect(model, &PowerModel::linePowerLidClosedActionChanged, this, [ = ] (const int reply) {
         setCloseLid(model, reply);
    });
    connect(model, &PowerModel::linePowerPressPowerBtnActionChanged, this, [ = ] (const int reply) {
         setPowerBtn(model, reply);
    });
    //----------------------------------------

    updatePowerButtonActionList();
    setCloseLid(model, model->linePowerLidClosedAction());
    setPowerBtn(model, model->linePowerPressPowerBtnAction());
}

void UseElectricWidget::setLidClose(bool state)
{
    m_cmbCloseLid->setVisible(state);
}

void UseElectricWidget::setScreenBlackDelayOnPower(const int delay)
{
    m_monitorSleepOnPower->slider()->blockSignals(true);
    m_monitorSleepOnPower->slider()->setValue(delay);
    m_monitorSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_monitorSleepOnPower->slider()->blockSignals(false);
}

void UseElectricWidget::setSleepDelayOnPower(const int delay)
{
    m_computerSleepOnPower->slider()->blockSignals(true);
    m_computerSleepOnPower->slider()->setValue(delay);
    m_computerSleepOnPower->setValueLiteral(delayToLiteralString(delay));
    m_computerSleepOnPower->slider()->blockSignals(false);
}

void UseElectricWidget::setLockScreenAfter(const int delay)
{
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseElectricWidget::setAutoLockScreenOnPower(const int delay)
{
    m_autoLockScreen->slider()->blockSignals(true);
    m_autoLockScreen->slider()->setValue(delay);
    m_autoLockScreen->setValueLiteral(delayToLiteralString(delay));
    m_autoLockScreen->slider()->blockSignals(false);
}

void UseElectricWidget::setCloseLid(const dcc::power::PowerModel *model, int lidIndex)
{
    Q_UNUSED(model);

    int tmpIndex = lidIndex;

    // 如果原来设置的关机，但是当前又不显示关机选项，则设置为无任何操作
    if (!model->getShutdown() && lidIndex == 0) {
        tmpIndex = m_cmbCloseLid->comboBox()->count() - 1;
    }

    // 如果原来设置的待机，但是当前不显示待机选项，则设置为无任何操作
    if (!model->getSuspend() && lidIndex == 1) {
        tmpIndex = m_cmbCloseLid->comboBox()->count() - 1;
    }

    // 如果原来设置的休眠，但是当前不显示休眠选项，则设置为无任何操作
    if (!model->getHibernate() && lidIndex == 2) {
        tmpIndex = m_cmbCloseLid->comboBox()->count() - 1;
    }

    // 设置为关闭显示器或无任何操作
    if (lidIndex == 3)
        tmpIndex = m_cmbCloseLid->comboBox()->count() - 2;
    else if (lidIndex == 4)
        tmpIndex = m_cmbCloseLid->comboBox()->count() - 1;

    if (tmpIndex < 0) return;

    m_cmbCloseLid->setCurrentIndex(tmpIndex);
}

void UseElectricWidget::setPowerBtn(const dcc::power::PowerModel *model, int powIndex)
{
    Q_UNUSED(model);

    int tmpIndex = powIndex;

    // 如果原来设置的关机，但是当前又不显示关机选项，则设置为无任何操作
    if (!model->getShutdown() && powIndex == 0) {
        tmpIndex = m_cmbPowerBtn->comboBox()->count() - 1;
    }

    // 如果原来设置的待机，但是当前不显示待机选项，则设置为无任何操作
    if (!model->getSuspend() && powIndex == 1) {
        tmpIndex = m_cmbPowerBtn->comboBox()->count() - 1;
    }

    // 如果原来设置的休眠，但是当前不显示休眠选项，则设置为无任何操作
    if (!model->getHibernate() && powIndex == 2) {
        tmpIndex = m_cmbPowerBtn->comboBox()->count() - 1;
    }

    // 设置为关闭显示器或无任何操作
    if (powIndex == 3)
        tmpIndex = m_cmbPowerBtn->comboBox()->count() - 2;
    else if (powIndex == 4)
        tmpIndex = m_cmbPowerBtn->comboBox()->count() - 1;

    if (tmpIndex < 0) return;

    m_cmbPowerBtn->setCurrentIndex(tmpIndex);
}

void UseElectricWidget::updatePowerButtonActionList()
{
    QStringList options;
    if (m_model->getShutdown()) {
        options << tr("Shut down");
    }
    if (m_model->getSuspend()) {
        options << tr("Suspend");
    }
    if (m_model->getHibernate()) {
        options << tr("Hibernate");
    }
    options << tr("Turn off the monitor") << tr("Do nothing");
    m_cmbPowerBtn->setComboxOption(options);
    m_cmbPowerBtn->addBackground();
    m_cmbCloseLid->setComboxOption(options);
    m_cmbCloseLid->addBackground();
}

QString UseElectricWidget::delayToLiteralString(const int delay) const
{
    QString strData = "";

    switch (delay) {
    case 1:
        strData = tr("1 Minute");
        break;
    case 2:
        strData = tr("%1 Minutes").arg(5);
        break;
    case 3:
        strData = tr("%1 Minutes").arg(10);
        break;
    case 4:
        strData = tr("%1 Minutes").arg(15);
        break;
    case 5:
        strData = tr("%1 Minutes").arg(30);
        break;
    case 6:
        strData = tr("1 Hour");
        break;
    case 7:
        strData = tr("Never");
        break;
    default:
        strData = tr("%1 Minutes").arg(15);
        break;
    }

    return strData;
}
