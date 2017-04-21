#include "mousewidget.h"
#include "switchwidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "widget/basesettings.h"
#include  "widget/mousesettings.h"
#include "widget/thinkpadsettings.h"
#include "mousemodel.h"
#include "mouse/model/mousemodelmousesettings.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;
using namespace dcc::mouse;
using namespace dcc::widgets;
MouseWidget::MouseWidget()
    : ModuleWidget()
{
    setObjectName("Mouse");

    m_baseSettings = new BaseSettings;
    m_centralLayout->addWidget(m_baseSettings);

    m_mouseSettings = new MouseSettings(tr("Mouse"));
    m_mouseSettings->setSwitchTitle(QString(tr("Disable the touchpad when inserting the mouse")));
    m_centralLayout->addWidget(m_mouseSettings);


    m_touchSettings = new MouseSettings(tr("TouchPad"));
    m_touchSettings->setSwitchTitle(QString(tr("Tap to Click")));
    m_centralLayout->addWidget(m_touchSettings);


    m_ThinkapdSettings = new ThinkpadSettings;
    m_centralLayout->addWidget(m_ThinkapdSettings);

    connect(m_baseSettings, &BaseSettings::requestSetLeftHand, this, &MouseWidget::requestSetLeftHand);
    connect(m_baseSettings, &BaseSettings::requestSetDisTyping, this, &MouseWidget::requestSetDisTyping);
    connect(m_mouseSettings, &MouseSettings::requestSetSwitch, this, &MouseWidget::requestSetDisTouchPad);
    connect(m_touchSettings, &MouseSettings::requestSetSwitch, this, &MouseWidget::requestSetTapClick);

    connect(m_baseSettings, &BaseSettings::requestSetSliderValue, this, &MouseWidget::requestSetDouClick);
    connect(m_mouseSettings, &MouseSettings::requestSetSliderValue, this, &MouseWidget::requestSetMouseMotionAcceleration);
    connect(m_touchSettings, &MouseSettings::requestSetSliderValue, this, &MouseWidget::requestSetTouchpadMotionAcceleration);
    connect(m_ThinkapdSettings, &ThinkpadSettings::requestSetSliderValue, this, &MouseWidget::requestSetTrackPointMotionAcceleration);

    connect(m_mouseSettings, &MouseSettings::requestSetNaturalScroll, this, &MouseWidget::requestSetMouseNaturalScroll);
    connect(m_touchSettings, &MouseSettings::requestSetNaturalScroll, this, &MouseWidget::requestSetTouchNaturalScroll);

    setTitle(tr("Mouse and Touchpad"));
}

void MouseWidget::setModel(MouseModel *const model)
{
    m_baseSettings->setModel(model->getBaseSettings());
    m_mouseSettings->setModel(model->getMouseSettings());
    m_touchSettings->setModel(model->getTouchSettings());
    m_ThinkapdSettings->setModel(model->getTrackSettings());

    m_touchpadModel = model->getTouchSettings();

    MouseModelMouseSettings *baseSettings = model->getTouchSettings();
    connect(baseSettings, &MouseModelMouseSettings::existChanged, this, &MouseWidget::onTouchpadVisibleChanged);

    onTouchpadVisibleChanged(baseSettings->getExist());

    MouseModelMouseSettings *mouseModel = model->getMouseSettings();
    connect(mouseModel, &MouseModelMouseSettings::switchChanged, this, &MouseWidget::onTouchpadHideChanged);

    onTouchpadHideChanged(mouseModel->getSwitchState());
}

void MouseWidget::onTouchpadVisibleChanged(const bool visible)
{
    m_baseSettings->setIsTypingVisible(visible);
    m_mouseSettings->setSwitchVisible(visible);
}

void MouseWidget::onTouchpadHideChanged(const bool visible)
{
    if (m_touchpadModel->getExist())
        m_touchSettings->setVisible(!visible);
}
