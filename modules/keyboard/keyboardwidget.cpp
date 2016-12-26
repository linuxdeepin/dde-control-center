#include "keyboardwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "titledslideritem.h"
#include "dccslider.h"
#include "searchinput.h"

using namespace dcc;
using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

KeyboardWidget::KeyboardWidget()
    :ModuleWidget(),
      m_bDelay(false)
{
    setObjectName("Keyboard");

    this->installEventFilter(parent());
    setTitle(tr("Keyboard and Language"));

    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    m_timer->setSingleShot(true);

    SettingsGroup *keyTest = new SettingsGroup();
    TitledSliderItem* delayItem =  new TitledSliderItem(tr("Repeat Delay"));
    m_delaySlider = delayItem->slider();
    m_delaySlider->setType(DCCSlider::Vernier);
    m_delaySlider->setOrientation(Qt::Horizontal);
    m_delaySlider->setRange(20, 600);
    m_delaySlider->setTickInterval(580/6);
    m_delaySlider->setTickPosition(QSlider::TicksBelow);
    QStringList delays;
    delays<<tr("Short")<<""<<""<<""<<""<<""<<tr("Long");
    delayItem->setAnnotations(delays);
    keyTest->appendItem(delayItem);

    TitledSliderItem* speedItem =  new TitledSliderItem(tr("Repeat Rate"));
    speedItem->setFixedHeight(speedItem->height() + 30);
    m_speedSlider = speedItem->slider();
    m_speedSlider->setType(DCCSlider::Vernier);
    m_speedSlider->setOrientation(Qt::Horizontal);
    m_speedSlider->setRange(200, 1000);
    m_speedSlider->setTickInterval((1000-200)/6);
    m_speedSlider->setTickPosition(QSlider::TicksBelow);
    QStringList speeds;
    speeds<<tr("Slow")<<""<<""<<""<<""<<""<<tr("Fast");
    speedItem->setAnnotations(speeds);
    SearchInput* testArea = new SearchInput();
    testArea->setFixedWidth(200);
    testArea->setSearchText(tr("Please Test Here"));
    testArea->setIconVisible(false);
    testArea->setStyleSheet("border: 0px");
    speedItem->layout()->addWidget(testArea);
    speedItem->layout()->setAlignment(testArea, Qt::AlignCenter);
    keyTest->appendItem(speedItem);

    m_upper = new SwitchWidget();
    m_upper->setTitle(tr("Caps Lock Prompt"));
    keyTest->appendItem(m_upper);

    SettingsGroup* keyGroup = new SettingsGroup();
    m_keyItem = new NextPageWidget();
    m_keyItem->setTitle(tr("Keyboard Layout"));
    m_keyItem->setValue(tr(""));
    keyGroup->appendItem(m_keyItem);

    SettingsGroup* langGroup = new SettingsGroup();
    m_langItem = new NextPageWidget();
    m_langItem->setTitle(tr("System Language"));
    langGroup->appendItem(m_langItem);

    SettingsGroup* scGroup = new SettingsGroup();
    m_scItem = new NextPageWidget();
    m_scItem->setTitle(tr("Shortcut"));
    scGroup->appendItem(m_scItem);

    m_centeralLayout->addWidget(keyTest);
    m_centeralLayout->addWidget(keyGroup);
    m_centeralLayout->addWidget(langGroup);
    m_centeralLayout->addWidget(scGroup);

    connect(m_keyItem,SIGNAL(clicked()), this, SIGNAL(keyoard()));
    connect(m_langItem, SIGNAL(clicked()), this, SIGNAL(language()));
    connect(m_scItem, SIGNAL(clicked()), this, SIGNAL(shortcut()));
    connect(m_delaySlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValue(int)));
    connect(m_speedSlider, SIGNAL(valueChanged(int)), this,  SLOT(onSliderValue(int)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(m_upper, SIGNAL(checkedChanegd(bool)), this, SIGNAL(capsLockChanged(bool)));
}

void KeyboardWidget::setKBValue(const QString &value)
{
    m_keyItem->setValue(value);
}

void KeyboardWidget::setLangValue(const QString &value)
{
    m_langItem->setValue(value);
}

void KeyboardWidget::onSliderValue(int value)
{
    Q_UNUSED(value);

    DCCSlider* slider = qobject_cast<DCCSlider*>(sender());
    m_bDelay = (slider == m_delaySlider);
    m_timer->start();
}

void KeyboardWidget::onTimeout()
{
    if(m_bDelay)
    {
        emit delayChanged(m_delaySlider->value());
    }
    else
    {
        emit speedChanged(m_speedSlider->value());
    }
}

void KeyboardWidget::setDelayValue(uint value)
{
    m_delaySlider->setValue(value);
}

void KeyboardWidget::setSpeedValue(uint value)
{
    m_speedSlider->setValue(1000 - (value * 10 - 200));
}

void KeyboardWidget::setCapsLock(bool value)
{
    m_upper->setChecked(value);
}
}
}
