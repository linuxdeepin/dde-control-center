#include "powerwidget.h"

#include <QDebug>
#include <QtMath>

#include "titledslideritem.h"
#include "dccslider.h"
#include "dccsliderannotated.h"
#include "optionitem.h"

using namespace dcc;
using namespace dcc::widgets;

static const int SliderMinValue = 0;
static const int SliderMaxValue = 600;
static const int SliderTickInterval = 100;

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
    m_monitorSleep->slider()->setRange(SliderMinValue, SliderMaxValue);
    m_monitorSleep->slider()->setTickPosition(QSlider::TicksBelow);
    m_monitorSleep->slider()->setTickInterval(SliderTickInterval);
    m_monitorSleep->setAnnotations(annos);

    m_computerSleep->slider()->setType(DCCSlider::Vernier);
    m_computerSleep->slider()->setRange(SliderMinValue, SliderMaxValue);
    m_computerSleep->slider()->setTickPosition(QSlider::TicksBelow);
    m_computerSleep->slider()->setTickInterval(SliderTickInterval);
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

    m_monitorSlider = m_monitorSleep->slider();
    m_sleepSlider = m_computerSleep->slider();

    connect(m_monitorSlider, &QSlider::valueChanged, this, &PowerWidget::monitorSliderValueChanged);
    connect(m_sleepSlider, &QSlider::valueChanged, this, &PowerWidget::sleepSliderValueChanged);
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
    blockSignals(true);
    m_displayNeedPassword->setChecked(model->screenBlackLock());
    m_wakeNeedPassword->setChecked(model->sleepLock());
    m_notebookSettings->setVisible(model->lidPresent());
    m_sleepOnLidOff->setChecked(model->sleepOnLidClose());
    blockSignals(false);

    setScreenBlackDelay(model->screenBlackDelay());
    setSleepDelay(model->sleepDelay());
}

int PowerWidget::delayToSliderValue(const int min, const int max, const int delay) const
{
    const float chunk = (max - min) / 6.f;
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
        return tr("%n Minutes", "", (delay / 60));
    }
}

int PowerWidget::sliderValueToDelay(const int min, const int max, const int tickInterval, const int value) const
{
    int tickPos = min;
    while (tickPos <= max) {
        if (tickPos - tickInterval / 2.f < value && value <= tickPos + tickInterval / 2.f) {
            break;
        }
        tickPos += tickInterval;
    }

    const int tickCount = qFloor((tickPos - min) / (1.f * tickInterval)) + 1;
    switch (tickCount) {
    case 1:
        return 60;
    case 2:
        return 60 * 5;
    case 3:
        return 60 * 10;
    case 4:
        return 60 * 15;
    case 5:
        return 60 * 30;
    case 6:
        return 60 * 60;
    default:
        return 0;
    }
}

void PowerWidget::setScreenBlackDelay(const int delay)
{
    m_monitorSlider->blockSignals(true);
    m_monitorSlider->setValue(delayToSliderValue(m_monitorSlider->minimum(), m_monitorSlider->maximum(), delay));
    m_monitorSleep->setValueLiteral(delayToLiteralString(delay));
    m_monitorSlider->blockSignals(false);
}

void PowerWidget::setSleepDelay(const int delay)
{
    m_sleepSlider->blockSignals(true);
    m_sleepSlider->setValue(delayToSliderValue(m_sleepSlider->minimum(), m_sleepSlider->maximum(), delay));
    m_computerSleep->setValueLiteral(delayToLiteralString(delay));
    m_sleepSlider->blockSignals(false);
}

void PowerWidget::monitorSliderValueChanged(const int delay)
{
    const int min = m_monitorSlider->minimum();
    const int max = m_monitorSlider->maximum();
    const int interval = m_monitorSlider->tickInterval();
    const int ret = sliderValueToDelay(min, max, interval, delay);

    emit requestSetScreenBlackDelay(ret);
}

void PowerWidget::sleepSliderValueChanged(const int delay)
{
    const int min = m_sleepSlider->minimum();
    const int max = m_sleepSlider->maximum();
    const int interval = m_sleepSlider->tickInterval();
    const int ret = sliderValueToDelay(min, max, interval, delay);

    emit requestSetSleepDelay(ret);
}
