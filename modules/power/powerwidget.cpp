#include "powerwidget.h"

#include <QDebug>
#include <QtMath>

#include "titledslideritem.h"
#include "dccslider.h"
#include "dccsliderannotated.h"
#include "optionitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::power;

PowerWidget::PowerWidget()
    : ModuleWidget(),

      m_sleepTimeoutSettings(new SettingsGroup),
      m_monitorSleep(new TitledSliderItem(tr("Monitor will suspend after"))),
      m_computerSleep(new TitledSliderItem(tr("Computer will suspend after"))),
      m_passwordSettings(new SettingsGroup),
      m_displayNeedPassword(new SwitchWidget),
      m_wakeNeedPassword(new SwitchWidget),
      m_notebookSettings(new SettingsGroup),
      m_sleepOnLidOff(new SwitchWidget)
{
    setObjectName("Power");

    QStringList annos;
    annos << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");

    m_monitorSleep->slider()->setType(DCCSlider::Vernier);
    m_monitorSleep->slider()->setRange(1, 7);
    m_monitorSleep->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleep->slider()->setTickInterval(1);
    m_monitorSleep->slider()->setPageStep(1);
    m_monitorSleep->setAnnotations(annos);

    m_computerSleep->slider()->setType(DCCSlider::Vernier);
    m_computerSleep->slider()->setRange(1, 7);
    m_computerSleep->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleep->slider()->setTickInterval(1);
    m_computerSleep->slider()->setPageStep(1);
    m_computerSleep->setAnnotations(annos);

    m_sleepTimeoutSettings->appendItem(m_monitorSleep);
    m_sleepTimeoutSettings->appendItem(m_computerSleep);

    m_displayNeedPassword->setTitle(tr("Password required to wake up the monitor"));
    m_passwordSettings->appendItem(m_displayNeedPassword);

    m_wakeNeedPassword->setTitle(tr("Password required to wake up the computer"));
    m_passwordSettings->appendItem(m_wakeNeedPassword);

    m_sleepOnLidOff->setTitle(tr("Suspend on lid close"));
    m_notebookSettings->appendItem(m_sleepOnLidOff);

    m_centralLayout->addWidget(m_sleepTimeoutSettings);
    m_centralLayout->addWidget(m_passwordSettings);
    m_centralLayout->addWidget(m_notebookSettings);

    setTitle(tr("Power Management"));

    connect(m_displayNeedPassword, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetScreenBlackLock);
    connect(m_wakeNeedPassword, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetSleepLock);
    connect(m_sleepOnLidOff, &SwitchWidget::checkedChanged, this, &PowerWidget::requestSetSleepOnLidClosed);

    m_monitorSlider = m_monitorSleep->slider();
    m_sleepSlider = m_computerSleep->slider();

    connect(m_monitorSlider, &QSlider::valueChanged, this, &PowerWidget::requestSetScreenBlackDelay);
    connect(m_sleepSlider, &QSlider::valueChanged, this, &PowerWidget::requestSetSleepDelay);

}

void PowerWidget::setModel(PowerModel *const model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_displayNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::sleepLockChanged, m_wakeNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::lidPresentChanged, m_notebookSettings, &SettingsGroup::setVisible);
    connect(model, &PowerModel::sleepOnLidCloseChanged, m_sleepOnLidOff, &SwitchWidget::setChecked);
    connect(model, &PowerModel::screenBlackDelayChanged, this, &PowerWidget::setScreenBlackDelay);
    connect(model, &PowerModel::sleepDelayChanged, this, &PowerWidget::setSleepDelay);

    // init ui data
    blockSignals(true);
    m_displayNeedPassword->setChecked(model->screenBlackLock());
    m_wakeNeedPassword->setChecked(model->sleepLock());
    m_notebookSettings->setVisible(model->lidPresent());
    m_sleepOnLidOff->setChecked(model->sleepOnLidClose());
    blockSignals(false);

    setScreenBlackDelay(model->screenBlackDelay());
    setSleepDelay(model->sleepDelay());
}

QString PowerWidget::delayToLiteralString(const int delay) const
{
    switch (delay) {
    case 1:
        return tr("1 Minute");
    case 2:
        return tr("%1 Minutes").arg(5);
    case 3:
        return tr("%1 Minutes").arg(10);
    case 4:
        return tr("%1 Minutes").arg(15);
    case 5:
        return tr("%1 Minutes").arg(30);
    case 6:
        return tr("1 Hour");
    case 7:
        return tr("Never");
    default:
        return tr("%1 Minutes").arg(15);
    }
}

void PowerWidget::setScreenBlackDelay(const int delay)
{
    m_monitorSlider->blockSignals(true);
    m_monitorSlider->setValue(delay);
    m_monitorSleep->setValueLiteral(delayToLiteralString(delay));
    m_monitorSlider->blockSignals(false);
}

void PowerWidget::setSleepDelay(const int delay)
{
    m_sleepSlider->blockSignals(true);
    m_sleepSlider->setValue(delay);
    m_computerSleep->setValueLiteral(delayToLiteralString(delay));
    m_sleepSlider->blockSignals(false);
}
