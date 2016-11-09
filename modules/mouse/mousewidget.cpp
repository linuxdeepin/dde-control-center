#include "mousewidget.h"

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;

MouseWidget::MouseWidget()
    : ModuleWidget(),

      m_userGroup(new SettingsGroup)
{
    m_centeralLayout->addWidget(m_userGroup);

    m_baseSettings = new BaseSettings;
    m_baseSettings->setStyleSheet("background-color:transparent;");
    m_centeralLayout->addWidget(m_baseSettings);

    m_mouseSettings = new MouseSettings;
    m_mouseSettings->setStyleSheet("background-color:transparent;");
    m_mouseSettings->setSwitchTitle(QString(tr("Disable the touchpad when inserting the mouse")));
    m_centeralLayout->addWidget(m_mouseSettings);


    m_touchSettings = new MouseSettings;
    m_touchSettings->setStyleSheet("background-color:transparent;");
    m_touchSettings->setSwitchTitle(QString(tr("Tap Click")));
    m_centeralLayout->addWidget(m_touchSettings);


    m_ThinkapdSettings = new ThinkpadSettings;
    m_ThinkapdSettings->setStyleSheet("background-color:transparent;");
    m_centeralLayout->addWidget(m_ThinkapdSettings);

    connect(m_baseSettings, &BaseSettings::requestSetLeftHand, this, &MouseWidget::requestSetLeftHand);
    connect(m_baseSettings, &BaseSettings::requestSetNaturalScroll, this, &MouseWidget::requestSetNaturalScroll);
    connect(m_baseSettings, &BaseSettings::requestSetDisTyping, this, &MouseWidget::requestSetDisTyping);
    connect(m_mouseSettings, &MouseSettings::requestSetSwitch, this, &MouseWidget::requestSetDisTouchPad);
    connect(m_touchSettings, &MouseSettings::requestSetSwitch, this, &MouseWidget::requestSetTapClick);

    connect(m_baseSettings, &BaseSettings::requestSetSliderValue, this, &MouseWidget::requestSetDouClick);
    connect(m_mouseSettings, &MouseSettings::requestSetSliderValue, this, &MouseWidget::requestSetMouseMotionAcceleration);
    connect(m_touchSettings, &MouseSettings::requestSetSliderValue, this, &MouseWidget::requestSetTouchpadMotionAcceleration);
    connect(m_ThinkapdSettings, &ThinkpadSettings::requestSetSliderValue, this, &MouseWidget::requestSetTrackPointMotionAcceleration);

    setTitle(tr("Mouse and Touchpad"));
}

void MouseWidget::setModel(MouseModel *const model)
{
    m_baseSettings->setModel(model->getBaseSettings());
    m_mouseSettings->setModel(model->getMouseSettings());
    m_touchSettings->setModel(model->getTouchSettings());
    m_ThinkapdSettings->setModel(model->getTrackSettings());
}
