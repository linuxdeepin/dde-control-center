#include <QDebug>
#include <QApplication>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>

#include <libdui/dslider.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dlineedit.h>
#include <libdui/dheaderline.h>
#include <libdui/dswitchbutton.h>

#include "moduleheader.h"
#include "normallabel.h"

#include "keyboard.h"
#include "dbus/dbusinputdevices.h"

DUI_USE_NAMESPACE

Keyboard::Keyboard() :
    QObject(),
    m_frame(new QFrame),
    m_dbusKeyboard(NULL)
{
    initBackend();
    if (m_dbusKeyboard) initUI();
}

Keyboard::~Keyboard()
{
    qDebug() << "~Keyboard and Language";
    delete m_frame;
}

void Keyboard::initBackend()
{
    DBusInputDevices * dbusInputDevices = new DBusInputDevices(this);
    foreach (InputDevice device, dbusInputDevices->infos()) {
        if (device.deviceType == "keyboard") {
            m_dbusKeyboard = new DBusKeyboard(this);
            break;
        }
    }
}

void Keyboard::initUI()
{
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    /// Header
    ModuleHeader * header = new ModuleHeader("Keyboard and Language");
    mainLayout->addWidget(header);
    mainLayout->addWidget(new DSeparatorHorizontal);
    connect(header, &ModuleHeader::resetButtonClicked, [=] {
        m_dbusKeyboard->Reset();
    });

    /// Basic Settings
    QGridLayout * basicSettingsLayout = new QGridLayout;
    basicSettingsLayout->setColumnStretch(0, 1);
    basicSettingsLayout->setColumnMinimumWidth(1, 180);
    basicSettingsLayout->setSpacing(20);
    basicSettingsLayout->setContentsMargins(0, 0, 14, 0);

    NormalLabel * repeatDelayTitle = new NormalLabel("Repeat Delay");
    DSlider * repeatDelaySlider = new DSlider(Qt::Horizontal);
    repeatDelaySlider->setRange(20, 600);
    repeatDelaySlider->setValue(m_dbusKeyboard->repeatDelay());
    basicSettingsLayout->addWidget(repeatDelayTitle, 0, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(repeatDelaySlider, 0, 1);

    connect(repeatDelaySlider, &DSlider::valueChanged, [=](int value){
        m_dbusKeyboard->setRepeatDelay(value);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::RepeatDelayChanged, [=]{
        repeatDelaySlider->setValue(m_dbusKeyboard->repeatDelay());
    });

    NormalLabel * repeatSpeedTitle = new NormalLabel("Repeat Rate");
    DSlider * repeatSpeedSlider = new DSlider(Qt::Horizontal);
    repeatSpeedSlider->setRange(200, 1000);
    repeatSpeedSlider->setValue(1000 - (m_dbusKeyboard->repeatInterval() * 10 - 200));
    basicSettingsLayout->addWidget(repeatSpeedTitle, 1, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(repeatSpeedSlider, 1, 1);

    connect(repeatSpeedSlider, &DSlider::valueChanged, [=](int value){
        m_dbusKeyboard->setRepeatInterval(((1000 - value) + 200) / 10);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::RepeatIntervalChanged, [=]{
        repeatSpeedSlider->setValue(1000 - (m_dbusKeyboard->repeatInterval() * 10 - 200));
    });

    QApplication * application = qobject_cast<QApplication*>(QApplication::instance());
    NormalLabel * cursorBlinkIntervalTitle = new NormalLabel("Cursor Blink Rate");
    DSlider * cursorBlinkIntervalSlider = new DSlider(Qt::Horizontal);
    cursorBlinkIntervalSlider->setRange(100, 2500);
    cursorBlinkIntervalSlider->setValue(2500 - (m_dbusKeyboard->cursorBlink() - 100));
    application->setCursorFlashTime(m_dbusKeyboard->cursorBlink());
    basicSettingsLayout->addWidget(cursorBlinkIntervalTitle, 2, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(cursorBlinkIntervalSlider, 2, 1);

    connect(cursorBlinkIntervalSlider, &DSlider::valueChanged, [=](int value){
        m_dbusKeyboard->setCursorBlink((2500 - value) + 100);
    });
    connect(m_dbusKeyboard, &DBusKeyboard::CursorBlinkChanged, [=]{
        cursorBlinkIntervalSlider->setValue(2500 - (m_dbusKeyboard->cursorBlink() - 100));
        application->setCursorFlashTime(m_dbusKeyboard->cursorBlink());
    });

    NormalLabel * testAreaTitle = new NormalLabel("Test Area");
    DLineEdit * testAreaEdit = new DLineEdit;
    basicSettingsLayout->addWidget(testAreaTitle, 3, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(testAreaEdit, 3, 1);

    mainLayout->addLayout(basicSettingsLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);

    DHeaderLine * capsLockLine = new DHeaderLine;
    DSwitchButton * capsLockSwitch = new DSwitchButton(capsLockLine);
    capsLockSwitch->setChecked(m_dbusKeyboard->capslockToggle());
    capsLockLine->setTitle("Caps Lock prompt");
    capsLockLine->setContent(capsLockSwitch);

    connect(capsLockSwitch, &DSwitchButton::checkedChanged, [=]{
        m_dbusKeyboard->setCapslockToggle(capsLockSwitch->checked());
    });

    connect(m_dbusKeyboard, &DBusKeyboard::CapslockToggleChanged, [=]{
        capsLockSwitch->setChecked(m_dbusKeyboard->capslockToggle());
    });

    mainLayout->addWidget(capsLockLine);
    mainLayout->addWidget(new DSeparatorHorizontal);

    mainLayout->addStretch(1);
}

QFrame* Keyboard::getContent()
{
    return m_frame;
}
