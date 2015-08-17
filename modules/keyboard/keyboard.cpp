#include <QDebug>
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

DUI_USE_NAMESPACE

Keyboard::Keyboard()
{
    initBackend();
    initUI();
}

Keyboard::~Keyboard()
{
    qDebug() << "~Keyboard and Language";
    delete m_frame;
}

void Keyboard::initBackend()
{
}

void Keyboard::initUI()
{
    m_frame = new QFrame;
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    /// Header
    ModuleHeader * header = new ModuleHeader("Keyboard and Language");
    mainLayout->addWidget(header);
    mainLayout->addWidget(new DSeparatorHorizontal);

    /// Basic Settings
    QGridLayout * basicSettingsLayout = new QGridLayout;
    basicSettingsLayout->setColumnStretch(0, 1);
    basicSettingsLayout->setColumnMinimumWidth(1, 180);
    basicSettingsLayout->setSpacing(20);
    basicSettingsLayout->setContentsMargins(0, 0, 14, 0);

    NormalLabel * repeatDelayTitle = new NormalLabel("Repeat Delay");
    DSlider * repeatDelaySlider = new DSlider(Qt::Horizontal);
    basicSettingsLayout->addWidget(repeatDelayTitle, 0, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(repeatDelaySlider, 0, 1);

    NormalLabel * repeatSpeedTitle = new NormalLabel("Repeat Rate");
    DSlider * repeatSpeedSlider = new DSlider(Qt::Horizontal);
    basicSettingsLayout->addWidget(repeatSpeedTitle, 1, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(repeatSpeedSlider, 1, 1);

    NormalLabel * cursorBlinkIntervalTitle = new NormalLabel("Cursor Blink Rate");
    DSlider * cursorBlinkIntervalSlider = new DSlider(Qt::Horizontal);
    basicSettingsLayout->addWidget(cursorBlinkIntervalTitle, 2, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(cursorBlinkIntervalSlider, 2, 1);

    NormalLabel * testAreaTitle = new NormalLabel("Test Area");
    DLineEdit * testAreaEdit = new DLineEdit;
    basicSettingsLayout->addWidget(testAreaTitle, 3, 0, Qt::AlignRight);
    basicSettingsLayout->addWidget(testAreaEdit, 3, 1);

    mainLayout->addLayout(basicSettingsLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);

    DHeaderLine * capsLockLine = new DHeaderLine;
    capsLockLine->setTitle("Caps Lock prompt");
    capsLockLine->setContent(new DSwitchButton);
    mainLayout->addWidget(capsLockLine);
    mainLayout->addWidget(new DSeparatorHorizontal);

    mainLayout->addStretch(1);
}

QFrame* Keyboard::getContent()
{
    return m_frame;
}
