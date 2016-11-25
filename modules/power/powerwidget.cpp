#include "powerwidget.h"

#include <QDebug>

#include "titledslideritem.h"
#include "dccslider.h"
#include "dccsliderannotated.h"

using namespace dcc;
using namespace dcc::widgets;

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
    QStringList annos;
    annos << "1m" << "5m" << "10m" << "15m" << "30m" << "1h" << tr("Never");

    m_monitorSleep->slider()->setType(DCCSlider::Vernier);
    m_monitorSleep->slider()->setRange(1, 100);
    m_monitorSleep->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleep->slider()->setTickInterval(100 / 6);
    m_monitorSleep->setAnnotations(annos);

    m_computerSleep->slider()->setType(DCCSlider::Vernier);
    m_computerSleep->slider()->setRange(1, 100);
    m_computerSleep->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleep->slider()->setTickInterval(100 / 6);
    m_computerSleep->setAnnotations(annos);

    m_sleepTimeoutSettings->appendItem(m_monitorSleep);
    m_sleepTimeoutSettings->appendItem(m_computerSleep);

    m_displayNeedPassword->setTitle(tr("Password required to wake the monitor"));
    m_passwordSettings->appendItem(m_displayNeedPassword);

    m_wakeNeedPassword->setTitle(tr("Password required to wake the computer"));
    m_passwordSettings->appendItem(m_wakeNeedPassword);

    m_sleepOnLidOff->setTitle(tr("Suspend on lid off"));
    m_notebookSettings->appendItem(m_sleepOnLidOff);

    m_centeralLayout->addWidget(m_sleepTimeoutSettings);
    m_centeralLayout->addSpacing(20);
    m_centeralLayout->addWidget(m_passwordSettings);

    m_centeralLayout->addSpacing(20);
    m_centeralLayout->addWidget(m_notebookSettings);

    setTitle(tr("Power Management"));

    connect(m_displayNeedPassword, &SwitchWidget::checkedChanegd, this, &PowerWidget::requestSetScreenBlackLock);
    connect(m_wakeNeedPassword, &SwitchWidget::checkedChanegd, this, &PowerWidget::requestSetSleepLock);
    connect(m_sleepOnLidOff, &SwitchWidget::checkedChanegd, this, &PowerWidget::requestSetSleepOnLidClosed);
}

void PowerWidget::setModel(PowerModel * const model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_displayNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::sleepLockChanged, m_wakeNeedPassword, &SwitchWidget::setChecked);
    connect(model, &PowerModel::lidPresentChanged, m_notebookSettings, &SettingsGroup::setVisible);
    connect(model, &PowerModel::sleepOnLidCloseChanged, m_sleepOnLidOff, &SwitchWidget::setChecked);
    connect(model, &PowerModel::screenBlackDelayChanged, this, &PowerWidget::setScreenBlackDelay);
    connect(model, &PowerModel::sleepDelayChanged, this, &PowerWidget::setSleepDelay);

    // init ui data
    m_displayNeedPassword->setChecked(model->screenBlackLock());
    m_wakeNeedPassword->setChecked(model->sleepLock());
    m_notebookSettings->setVisible(model->lidPresent());
    m_sleepOnLidOff->setChecked(model->sleepOnLidClose());

    setScreenBlackDelay(model->screenBlackDelay());
    setSleepDelay(model->sleepDelay());
}

int PowerWidget::delayToSliderValue(const int min, const int max, const int delay) const
{
    static const float chunk = (max - min) / 6.f;
    if (delay == 60) {
        return min;
    } else if (delay == 60 * 5) {
        return min + chunk;
    } else if (delay == 60 * 10) {
        return min + chunk * 2;
    } else if (delay == 60 * 15) {
        return min + chunk * 3;
    } else if (delay == 60 * 30) {
        return min + chunk * 4;
    } else if (delay == 60 * 60) {
        return min + chunk * 5;
    }

    return max;
}

QString PowerWidget::delayToLiteralString(const int delay) const
{
    if (delay == 60 * 60) {
        return tr("1 Hour");
    } else if (delay == 0) {
        return tr("Never");
    } else {
        return tr("%1 Minutes").arg(delay / 60);
    }
}

void PowerWidget::setScreenBlackDelay(const int delay)
{
    QSlider *monitorSlider = m_monitorSleep->slider();
    monitorSlider->setValue(delayToSliderValue(monitorSlider->minimum(), monitorSlider->maximum(), delay));
    m_monitorSleep->setValueLiteral(delayToLiteralString(delay));
}

void PowerWidget::setSleepDelay(const int delay)
{
    QSlider *sleepSlider = m_computerSleep->slider();
    sleepSlider->setValue(delayToSliderValue(sleepSlider->minimum(), sleepSlider->maximum(), delay));
    m_computerSleep->setValueLiteral(delayToLiteralString(delay));
}
