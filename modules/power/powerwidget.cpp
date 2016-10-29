#include "powerwidget.h"

#include <QDebug>

using namespace dcc;

PowerWidget::PowerWidget()
    : ModuleWidget(),

      m_suspendSettings(new SettingsGroup),
      m_displayNeedPassword(new SwitchWidget),
      m_wakeNeedPassword(new SwitchWidget)
{
    m_displayNeedPassword->setTitle(tr("Password required to wake the monitor"));
    m_suspendSettings->appendItem(m_displayNeedPassword);

    m_wakeNeedPassword->setTitle(tr("Password required to wake the computer"));
    m_suspendSettings->appendItem(m_wakeNeedPassword);

    m_centeralLayout->addWidget(m_suspendSettings);

    for (int i(0); i != 10; ++i)
    {
        SwitchWidget *w = new SwitchWidget;
        m_suspendSettings->appendItem(w);
    }

    setTitle(tr("Power"));

    connect(m_displayNeedPassword, &SwitchWidget::checkedChanegd, this, &PowerWidget::requestSetScreenBlackLock);
}

void PowerWidget::setModel(PowerModel * const model)
{
    connect(model, &PowerModel::screenBlackLockChanged, m_displayNeedPassword, &SwitchWidget::setChecked);

    // init ui data
    m_displayNeedPassword->setChecked(model->screenBlackLock());
}
