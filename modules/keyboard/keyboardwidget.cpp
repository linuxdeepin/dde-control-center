#include "keyboardwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "titledslideritem.h"
#include "dccslider.h"
#include "searchinput.h"
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
    speedItem->setFixedHeight(speedItem->height() + 30);
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
    speedItem->layout()->addWidget(testArea);
    speedItem->layout()->setAlignment(testArea, Qt::AlignCenter);
    speedItem->setContentsMargins(0, 0, 0, 10);
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

    m_centralLayout->addWidget(keyTest);
    m_centralLayout->addWidget(keyGroup);
    m_centralLayout->addWidget(langGroup);
    m_centralLayout->addWidget(scGroup);

    connect(m_keyItem,SIGNAL(clicked()), this, SIGNAL(keyoard()));
    connect(m_langItem, SIGNAL(clicked()), this, SIGNAL(language()));
    connect(m_scItem, SIGNAL(clicked()), this, SIGNAL(shortcut()));
    connect(m_delaySlider, &DCCSlider::valueChanged, this, &KeyboardWidget::delayChanged);
    connect(m_speedSlider, &DCCSlider::valueChanged, this, &KeyboardWidget::speedChanged);
    connect(m_upper, SIGNAL(checkedChanged(bool)), this, SIGNAL(capsLockChanged(bool)));


    connect(m_model, &KeyboardModel::repeatDelayChanged, this, &KeyboardWidget::setDelayValue);
    connect(m_model, &KeyboardModel::repeatIntervalChanged, this, &KeyboardWidget::setSpeedValue);
    connect(m_model, &KeyboardModel::capsLockChanged,m_upper, &SwitchWidget::setChecked);
    connect(m_model, &KeyboardModel::curLayoutChanged, m_keyItem, &NextPageWidget::setValue);
    connect(m_model, &KeyboardModel::curLangChanged, m_langItem, &NextPageWidget::setValue);

    setDelayValue(m_model->repeatDelay());
    setSpeedValue(m_model->repeatInterval());
    m_upper->setChecked(m_model->capsLock());
    m_keyItem->setValue(m_model->curLayout());
    m_langItem->setValue(m_model->curLang());
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
