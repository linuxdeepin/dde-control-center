#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include "constants.h"

#include "mouse.h"

DUI_USE_NAMESPACE


/// as long as the qslider can't have float value,
/// 	here , we have some value expanded. So pay attention.


Mouse::Mouse()
{
    m_label = new QLabel();
    m_label->setStyleSheet(QString("QLabel{color: %1;font-size:12px;}").arg(DCC::TextNormalColor.name()));
    QVBoxLayout * layout = new QVBoxLayout(m_label);
    layout->setMargin(0);

    m_mouseInterface = new ComDeepinDaemonInputDeviceMouseInterface("com.deepin.daemon.InputDevices",
                                                                    "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this);
    m_touchpadInterface = new ComDeepinDaemonInputDeviceTouchPadInterface("com.deepin.daemon.InputDevices",
                                                                          "/com/deepin/daemon/InputDevice/TouchPad", QDBusConnection::sessionBus(), this);
    //////////////////////////////////////////////////////////////-- top header
    m_topHeaderLine = new DHeaderLine(m_label);
    m_mouseResetButton = new DTextButton(tr("Reset"));
    m_topHeaderLine->setTitle(tr("Mouse"));
    m_topHeaderLine->setContent(m_mouseResetButton);

    //////////////////////////////////////////////////////////////-- horizontal separator
    m_firstHSeparator = new DSeparatorHorizontal(m_label);

    //////////////////////////////////////////////////////////////-- mouse setting panel
    m_mouseSettingPanel = new QWidget(m_label);
    m_mouseSettingPanelLayout = new QVBoxLayout(m_mouseSettingPanel);
    m_mouseSettingPanel->setLayout(m_mouseSettingPanelLayout);

    /// primary button setting part
    m_primaryButtonContainer = new QWidget(m_mouseSettingPanel);
    m_primaryButtonContainerLayout = new QHBoxLayout(m_primaryButtonContainer);
    m_primaryButtonContainer->setLayout(m_primaryButtonContainerLayout);
    m_primaryButtonContainerLayout->setMargin(0);

    m_primaryButtonSetting = new QLabel(tr("Primary Button"));

    m_mousePrimaryButtonSetting = new DSegmentedControl(m_primaryButtonContainer);
    m_mousePrimaryButtonSetting->addSegmented(tr("Left Button"));
    m_mousePrimaryButtonSetting->addSegmented(tr("Right Button"));

    m_primaryButtonContainerLayout->addWidget(m_primaryButtonSetting);
    m_primaryButtonContainerLayout->addWidget(m_mousePrimaryButtonSetting);
    /// point speed setting part
    m_pointSpeedContainer = new QWidget(m_mouseSettingPanel);
    m_pointSpeedContainerLayout = new QHBoxLayout(m_pointSpeedContainer);
    m_pointSpeedContainer->setLayout(m_pointSpeedContainerLayout);
    m_pointSpeedContainerLayout->setMargin(0);

    m_pointSpeedLabel = new QLabel(tr("Point Speed"));
    m_mousePointSpeedSlider = new DSlider(Qt::Horizontal);
    m_mousePointSpeedSlider->setMinimumSize(180, 20);

    m_pointSpeedContainerLayout->addWidget(m_pointSpeedLabel);
    m_pointSpeedContainerLayout->addWidget(m_mousePointSpeedSlider);

    /// double click setting part
    m_doubleClickSpeedContainer = new QWidget(m_mouseSettingPanel);
    m_doubleClickContainerLayout = new QHBoxLayout(m_doubleClickSpeedContainer);
    m_doubleClickSpeedContainer->setLayout(m_doubleClickContainerLayout);
    m_doubleClickContainerLayout->setMargin(0);

    m_doubleClickSpeedLabel = new QLabel(tr("Double Click Speed"));
    m_mouseDoubleClickIntervalSlider = new DSlider(Qt::Horizontal);
    m_mouseDoubleClickIntervalSlider->setMinimumSize(180, 20);

    m_doubleClickContainerLayout->addWidget(m_doubleClickSpeedLabel);
    m_doubleClickContainerLayout->addWidget(m_mouseDoubleClickIntervalSlider);


    //////////////////////////////////////////////////////////////-- mouse or touchpad
    m_forbiddenTouchpadWhenMouseContainer = new QWidget(m_mouseSettingPanel);
    m_forbiddenTouchpadWhenMouseLayout = new QHBoxLayout(m_forbiddenTouchpadWhenMouseContainer);
    m_forbiddenTouchpadWhenMouseContainer->setLayout(m_forbiddenTouchpadWhenMouseLayout);
    // TODO: label has too much text, font may need to be set
    m_forbiddenTouchpadWhenMouseLabel = new QLabel(tr("Disable touchpad when"
                                                             " the mouse is plugged in"));
    m_forbiddenTouchpadWhenMouseSwitchButton = new DSwitchButton(m_forbiddenTouchpadWhenMouseContainer);
    m_forbiddenTouchpadWhenMouseSwitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_forbiddenTouchpadWhenMouseLayout->addWidget(m_forbiddenTouchpadWhenMouseLabel);
    m_forbiddenTouchpadWhenMouseLayout->addStretch(0);
    m_forbiddenTouchpadWhenMouseLayout->addWidget(m_forbiddenTouchpadWhenMouseSwitchButton);
    m_forbiddenTouchpadWhenMouseLayout->addSpacing(4);
    m_forbiddenTouchpadWhenMouseLayout->setMargin(0);

    ///
    m_mouseSettingPanelLayout->addWidget(m_primaryButtonContainer);
    m_mouseSettingPanelLayout->addSpacing(10);
    m_mouseSettingPanelLayout->addWidget(m_pointSpeedContainer);
    m_mouseSettingPanelLayout->addSpacing(10);
    m_mouseSettingPanelLayout->addWidget(m_doubleClickSpeedContainer);
    m_mouseSettingPanelLayout->addSpacing(10);
    m_mouseSettingPanelLayout->addWidget(m_forbiddenTouchpadWhenMouseContainer);
    m_mouseSettingPanelLayout->addSpacing(10);

    //////////////////////////////////////////////////////////////-- horizontal separator
    m_secondHSeparator = new DSeparatorHorizontal(m_label);

    //////////////////////////////////////////////////////////////-- touchpad header line
    m_touchpadHeaderLine = new DHeaderLine(m_label);
    m_touchpadSwitchButton = new DSwitchButton(m_touchpadHeaderLine);
    m_touchpadHeaderLine->setTitle(tr("Touchpad"));
    m_touchpadHeaderLine->setContent(m_touchpadSwitchButton);
    m_touchpadSwitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //////////////////////////////////////////////////////////////-- horizontal separator
    m_thirdHSeparator = new DSeparatorHorizontal(m_label);

    //////////////////////////////////////////////////////////////-- touchpad setting panel
    m_touchpadSettingPanel = new ContainerWidget(m_label);

    m_touchpadPrimaryButtonSetting = new DSegmentedControl(m_touchpadSettingPanel);
    m_touchpadPrimaryButtonSetting->addSegmented(tr("Left Button"));
    m_touchpadPrimaryButtonSetting->addSegmented(tr("Right Button"));

    m_touchpadPointSpeed = new DSlider(Qt::Horizontal);
    m_touchpadDoubleClickSpeed = new DSlider(Qt::Horizontal);
    m_touchpadDragThreshold = new DSlider(Qt::Horizontal);

    m_touchpadPointSpeed->setMinimumSize(180, 20);
    m_touchpadDoubleClickSpeed->setMinimumSize(180, 20);
    m_touchpadDragThreshold->setMinimumSize(180, 20);

    m_touchpadNatureScrollSwitch = new DSwitchButton();
    m_touchpadTapToClickSwitch = new DSwitchButton();
    m_touchpadTwoFingerScrollSwitch = new DSwitchButton();
    m_touchpadEdgeScrollSwitch = new DSwitchButton();

    m_touchpadNatureScrollSwitch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_touchpadTapToClickSwitch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_touchpadTwoFingerScrollSwitch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_touchpadEdgeScrollSwitch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_touchpadSettingPanel->addRow(tr("Primary Button"), 0, m_touchpadPrimaryButtonSetting);
    m_touchpadSettingPanel->addRow(tr("Tracking Speed"), m_touchpadPointSpeed);
    m_touchpadSettingPanel->addRow(tr("Double Click Speed"), m_touchpadDoubleClickSpeed);
    m_touchpadSettingPanel->addRow(tr("Drag Thredshold"), m_touchpadDragThreshold);
    m_touchpadSettingPanel->addRow(tr("Nature Scroll"), 0, m_touchpadNatureScrollSwitch);
    m_touchpadSettingPanel->addRow(tr("Tap To Click"), 0, m_touchpadTapToClickSwitch);
    m_touchpadSettingPanel->addRow(tr("Two Finger Scroll"), 0, m_touchpadTwoFingerScrollSwitch);
    m_touchpadSettingPanel->addRow(tr("Edge Scroll"), 0, m_touchpadEdgeScrollSwitch);

    //////////////////////////////////////////////////////////////-- horizontal separator
    m_fourthHSeparator = new DSeparatorHorizontal(m_label);

    //////////////////////////////////////////////////////////////
    // normalize the style of those labels , buttons
    // TODO: maybe we can make this kind of label a single class, and so as container

    int labelWidth = 100;
    m_primaryButtonSetting->setFixedWidth(labelWidth);
    m_pointSpeedLabel->setFixedWidth(labelWidth);
    m_doubleClickSpeedLabel->setFixedWidth(labelWidth);
    m_forbiddenTouchpadWhenMouseLabel->setFixedWidth(labelWidth);

    m_primaryButtonSetting->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_pointSpeedLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_doubleClickSpeedLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_forbiddenTouchpadWhenMouseLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    m_doubleClickSpeedLabel->setWordWrap(true);
    m_forbiddenTouchpadWhenMouseLabel->setWordWrap(true);

    layout->addWidget(m_topHeaderLine);
    layout->addWidget(m_firstHSeparator);
    layout->addWidget(m_mouseSettingPanel);
    layout->addWidget(m_secondHSeparator);
    layout->addWidget(m_touchpadHeaderLine);
    layout->addWidget(m_thirdHSeparator);
    layout->addWidget(m_touchpadSettingPanel);
    layout->addWidget(m_fourthHSeparator);
    layout->addStretch(1);


    ////////////////////////////////////////////////////////////// init those widgets state
    setWidgetsValue();
    ////////////////////////////////////////////////////////////// init those widgets state
    connect(m_mouseResetButton, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(m_mousePrimaryButtonSetting, SIGNAL(currentChanged(int)), this, SLOT(setMousePrimaryButton(int)));
    connect(m_mousePointSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(setMousePointSpeed(int)));
    connect(m_mouseDoubleClickIntervalSlider, SIGNAL(valueChanged(int)), this, SLOT(setMouseDoubleClickInterval(int)));

    connect(m_forbiddenTouchpadWhenMouseSwitchButton, SIGNAL(checkedChanged(bool)),
            this, SLOT(disableTouchpadWhenMousePluggedIn(bool)));

    connect(m_touchpadSwitchButton, SIGNAL(checkedChanged(bool)), this, SLOT(enableTouchpad(bool)));
    connect(m_touchpadPointSpeed, SIGNAL(valueChanged(int)), this, SLOT(setTouchpadPointSpeed(int)));
    connect(m_touchpadDoubleClickSpeed, SIGNAL(valueChanged(int)), this, SLOT(setTouchpadDoubleClickInterval(int)));
    connect(m_touchpadNatureScrollSwitch, SIGNAL(checkedChanged(bool)), this, SLOT(enableTouchpadNatureScroll(bool)));
    connect(m_touchpadTapToClickSwitch, SIGNAL(checkedChanged(bool)), this, SLOT(enableTouchpadTapToClick(bool)));
    connect(m_touchpadTwoFingerScrollSwitch, SIGNAL(checkedChanged(bool)), this, SLOT(enableTouchpadEdgeScroll(bool)));
    connect(m_touchpadEdgeScrollSwitch, SIGNAL(checkedChanged(bool)), this, SLOT(enableTouchpadEdgeScroll(bool)));
}

void Mouse::reset() {
    QDBusPendingCall reply = m_mouseInterface->Reset();
    reply.waitForFinished();
    setWidgetsValue();
}

void Mouse::setWidgetsValue() {
    m_mouseInterface->deviceList().printValue();
    if (m_mouseInterface->leftHanded() == true){
        m_mousePrimaryButtonSetting->setCurrentIndex(1);
    } else {
        m_mousePrimaryButtonSetting->setCurrentIndex(0);
    }

    m_mousePointSpeedSlider->setRange(2, 30);	// 0.2 ~ 3
    m_mousePointSpeedSlider->setValue(m_mouseInterface->motionAcceleration() * 10);

    m_mouseDoubleClickIntervalSlider->setRange(100, 1000);	// 100 ~ 1000
    m_mouseDoubleClickIntervalSlider->setValue(m_mouseInterface->doubleClick());

    m_forbiddenTouchpadWhenMouseSwitchButton->setChecked(m_mouseInterface->disableTpad());

    if (m_touchpadInterface->tPadEnable()){
        m_touchpadSwitchButton->setChecked(true);
    } else {
        m_touchpadSwitchButton->setChecked(false);
    }

    if (m_touchpadInterface->leftHanded() == true){
        m_touchpadPrimaryButtonSetting->setCurrentIndex(1);
    } else {
        m_touchpadPrimaryButtonSetting->setCurrentIndex(0);
    }

    m_touchpadPointSpeed->setRange(2, 30);	// not sure
    m_touchpadPointSpeed->setValue(m_touchpadInterface->motionAcceleration() * 10);

    m_touchpadDoubleClickSpeed->setRange(100, 1000);	// not sure
    m_touchpadDoubleClickSpeed->setValue(m_touchpadInterface->doubleClick());

    m_touchpadDragThreshold->setRange(2, 30);	// not sure
    m_touchpadDragThreshold->setValue(m_touchpadInterface->dragThreshold());

    m_touchpadNatureScrollSwitch->setChecked(m_touchpadInterface->naturalScroll());
    m_touchpadTapToClickSwitch->setChecked(m_touchpadInterface->tapClick());
    // touchpadTwoFingerScrollSwitch->setChecked(m_touchpadInterface->);
    m_touchpadEdgeScrollSwitch->setChecked(m_touchpadInterface->edgeScroll());

}

void Mouse::setMousePrimaryButton(int index)
{
    if (index == 0){
        m_mouseInterface->setLeftHanded(false);
    } else {
        m_mouseInterface->setLeftHanded(true);
    }
}

void Mouse::setMousePointSpeed(int speed)
{
    // the value should be scaled
    m_mouseInterface->setMotionAcceleration((3 - speed * 1.0 / 10) + 0.2);
}

void Mouse::setMouseDoubleClickInterval(int interval)
{
    m_mouseInterface->setDoubleClick(interval);
}

void Mouse::disableTouchpadWhenMousePluggedIn(bool flag)
{
    m_mouseInterface->setDisableTpad(flag);
}

void Mouse::enableTouchpad(bool flag)
{
    m_touchpadInterface->setTPadEnable(flag);
}

void Mouse::setTouchpadPointSpeed(int speed)
{
    m_touchpadInterface->setMotionAcceleration(speed * 1.0 / 30);
}

void Mouse::setTouchpadDoubleClickInterval(int interval)
{
    m_touchpadInterface->setDoubleClick(interval);
}

void Mouse::enableTouchpadNatureScroll(bool flag)
{
    m_touchpadInterface->setDoubleClick(flag);
}

void Mouse::enableTouchpadTapToClick(bool flag)
{
    m_touchpadInterface->setTapClick(flag);
}

void Mouse::enableTouchpadTwoFingerScroll(bool flag)
{
    Q_UNUSED(flag)
    qDebug() << "tmp not implemented";
    //
}

void Mouse::enableTouchpadEdgeScroll(bool flag)
{
    m_touchpadInterface->setEdgeScroll(flag);
}

Mouse::~Mouse()
{
    qDebug() << "~Mouse()";

    m_label->deleteLater();
}

QFrame* Mouse::getContent()
{
    return m_label;
}


ContainerWidget::ContainerWidget(QWidget *parent)
    :QWidget(parent)
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    setLayout(layout);

    m_labelWidth = 100;
    m_rowHeight = 35;
}

ContainerWidget::~ContainerWidget()
{
}

void ContainerWidget::addRow(const QString & text, QWidget * widget)
{
    QWidget * container = new QWidget(this);
    layout()->addWidget(container);
    container->setMinimumHeight(m_rowHeight);

    QHBoxLayout * containerLayout = new QHBoxLayout(container);
    container->setLayout(containerLayout);

    QLabel * label = new QLabel(text);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label->setWordWrap(true);
    label->setFixedWidth(m_labelWidth);

    containerLayout->addWidget(label);
    containerLayout->addWidget(widget);
}

void ContainerWidget::addRow(const QString & text, int stretch, QWidget * widget, int spacing)
{
    QWidget * container = new QWidget(this);
    layout()->addWidget(container);
    container->setMinimumHeight(m_rowHeight);

    QHBoxLayout * containerLayout = new QHBoxLayout(container);
    container->setLayout(containerLayout);

    QLabel * label = new QLabel(text);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label->setFixedWidth(m_labelWidth);
    label->setWordWrap(true);

    containerLayout->addWidget(label);
    containerLayout->addStretch(stretch);
    containerLayout->addWidget(widget);
    containerLayout->addSpacing(spacing);

    containerLayout->setMargin(0);
}
