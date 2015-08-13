#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dtextbutton.h>
#include <libdui/dswitchlineexpand.h>
#include <libdui/dslider.h>
#include <libdui/dbuttonlist.h>

#include "sound.h"
#include "moduleheader.h"
#include "headerline.h"
#include "normallabel.h"

DUI_USE_NAMESPACE

Sound::Sound()
{
    m_frame = new QFrame;
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    initUI();
}

Sound::~Sound()
{
    qDebug() << "~Sound";
    delete m_frame;
}

void Sound::initUI()
{
    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    ///////////////////////////////////////////////////////-- Title
    ModuleHeader * header = new ModuleHeader("Sound");
    mainLayout->addWidget(header);
    mainLayout->addWidget(new DSeparatorHorizontal);

    ///////////////////////////////////////////////////////-- Speaker Settings
    DSwitchLineExpand * speakerExpand = new DSwitchLineExpand;
    speakerExpand->setTitle("Speaker");

    QFrame * speakerExpandContent = new QFrame(speakerExpand);
    speakerExpandContent->setFixedWidth(310);
    speakerExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout * speakerForm = new QGridLayout(speakerExpandContent);
    speakerForm->setRowMinimumHeight(0, 36);
    speakerForm->setRowMinimumHeight(1, 36);

    // Output volume line
    speakerForm->addWidget(new NormalLabel("Output volume"), 0, 0, Qt::AlignVCenter);
    DSlider * outputVolumeSlider = new DSlider(Qt::Horizontal);
    speakerForm->addWidget(outputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // Left/Right balance line
    speakerForm->addWidget(new NormalLabel("Left/Right balance"), 1, 0, Qt::AlignVCenter);
    DSlider * leftRightBalanceSlider = new DSlider(Qt::Horizontal);
    leftRightBalanceSlider->setHandleType(DSlider::SharpHandler);
    speakerForm->addWidget(leftRightBalanceSlider, 1, 1, Qt::AlignVCenter);

    speakerExpandContent->adjustSize();
    speakerExpand->setContent(speakerExpandContent);
    mainLayout->addWidget(speakerExpand);
    mainLayout->addWidget(new DBaseLine);

    ///////////////////////////////////////////////////////-- Microphone Settings
    DSwitchLineExpand * mircophoneExpand = new DSwitchLineExpand;
    mircophoneExpand->setTitle("Microphone");

    QFrame * mircophoneExpandContent = new QFrame(mircophoneExpand);
    mircophoneExpandContent->setFixedWidth(310);
    mircophoneExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout * microphoneForm = new QGridLayout(mircophoneExpandContent);
    microphoneForm->setRowMinimumHeight(0, 36);
    microphoneForm->setRowMinimumHeight(1, 36);

    // microphone volume line
    microphoneForm->addWidget(new NormalLabel("Input volume"), 0, 0, Qt::AlignVCenter);
    DSlider * inputVolumeSlider = new DSlider(Qt::Horizontal);
    microphoneForm->addWidget(inputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // microphone feedback line
    microphoneForm->addWidget(new NormalLabel("Feedback volume"), 1, 0, Qt::AlignVCenter);
    DSlider * inputFeedbackSlider = new DSlider(Qt::Horizontal);
    microphoneForm->addWidget(inputFeedbackSlider, 1, 1, Qt::AlignVCenter);

    mircophoneExpandContent->adjustSize();
    mircophoneExpand->setContent(mircophoneExpandContent);
    mainLayout->addWidget(mircophoneExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(new DBaseLine);

    ///////////////////////////////////////////////////////--Advanced settings
    // Output ports
    DBaseExpand * outputPortsExpand = new DBaseExpand;

    HeaderLine * outputPortsLine = new HeaderLine("Output ports", outputPortsExpand);
    outputPortsExpand->setHeader(outputPortsLine);

    QStringList outputPorts;
    outputPorts << "Port1" << "Port2";
    DButtonList * outputPortsList = new DButtonList(outputPortsExpand);
    outputPortsList->addButtons(outputPorts);
    outputPortsList->setFixedSize(310, outputPorts.length() * outputPortsList->itemWidget(outputPortsList->item(0))->height());

    outputPortsExpand->setContent(outputPortsList);

    mainLayout->addWidget(outputPortsExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // Output devices
    DBaseExpand * outputDevicesExpand = new DBaseExpand;

    HeaderLine * outputDevicesLine = new HeaderLine("Output ports", outputDevicesExpand);
    outputDevicesExpand->setHeader(outputDevicesLine);

    QStringList outputDevices;
    outputDevices << "Port1" << "Port2";
    DButtonList * outputDevicesList = new DButtonList(outputDevicesExpand);
    outputDevicesList->addButtons(outputDevices);
    outputDevicesList->setFixedSize(310, outputDevices.length() * outputDevicesList->itemWidget(outputDevicesList->item(0))->height());

    outputDevicesExpand->setContent(outputDevicesList);

    mainLayout->addWidget(outputDevicesExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(new DBaseLine);

    // Input ports
    DBaseExpand * inputPortsExpand = new DBaseExpand;

    HeaderLine * inputPortsLine = new HeaderLine("Input ports", inputPortsExpand);
    inputPortsExpand->setHeader(inputPortsLine);

    QStringList inputPorts;
    inputPorts << "Port1" << "Port2";
    DButtonList * inputPortsList = new DButtonList(inputPortsExpand);
    inputPortsList->addButtons(inputPorts);
    inputPortsList->setFixedSize(310, inputPorts.length() * inputPortsList->itemWidget(inputPortsList->item(0))->height());

    inputPortsExpand->setContent(inputPortsList);

    mainLayout->addWidget(inputPortsExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // Input devices
    DBaseExpand * inputDevicesExpand = new DBaseExpand;

    HeaderLine * inputDevicesLine = new HeaderLine("Input ports", inputDevicesExpand);
    inputDevicesExpand->setHeader(inputDevicesLine);

    QStringList inputDevices;
    inputDevices << "Port1" << "Port2";
    DButtonList * inputDevicesList = new DButtonList(inputDevicesExpand);
    inputDevicesList->addButtons(inputDevices);
    inputDevicesList->setFixedSize(310, inputDevices.length() * inputDevicesList->itemWidget(inputDevicesList->item(0))->height());

    inputDevicesExpand->setContent(inputDevicesList);

    mainLayout->addWidget(inputDevicesExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    mainLayout->addStretch(1);
}

QFrame* Sound::getContent()
{
    qDebug() << "Sound module" << "getContent";
    return m_frame;
}
