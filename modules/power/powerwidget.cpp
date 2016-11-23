#include "powerwidget.h"

#include <QDebug>

#include "titledslideritem.h"

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
}

void PowerWidget::setModel(PowerModel * const model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_displayNeedPassword, &SwitchWidget::setChecked);

    // init ui data
    m_displayNeedPassword->setChecked(model->screenBlackLock());
}
