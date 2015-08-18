#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDBusObjectPath>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dtextbutton.h>
#include <libdui/dswitchlineexpand.h>
#include <libdui/dslider.h>
#include <libdui/dbuttonlist.h>

#include "sound.h"
#include "moduleheader.h"
#include "headerline.h"
#include "normallabel.h"

#include "dbus/dbusaudio.h"

DUI_USE_NAMESPACE

Sound::Sound() :
    QObject()
{
    initBackend();
    initUI();
}

Sound::~Sound()
{
    qDebug() << "~Sound";
    delete m_frame;
}

void Sound::initBackend()
{
    DBusAudio * audio = new DBusAudio(this);

    QDBusPendingReply<QDBusObjectPath> reply1 = audio->GetDefaultSink();
    reply1.waitForFinished();
    QDBusObjectPath sink = reply1.value();
    m_sink = new DBusAudioSink(sink.path(), this);

    QDBusPendingReply<QDBusObjectPath> reply2 = audio->GetDefaultSource();
    reply2.waitForFinished();
    QDBusObjectPath source = reply2.value();
    m_source = new DBusAudioSource(source.path(), this);
}

void Sound::initUI()
{
    m_frame = new QFrame;
    m_frame->setFixedWidth(310);

    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setSpacing(0);
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
    outputVolumeSlider->setTracking(false);
    outputVolumeSlider->setRange(0, 150);
    outputVolumeSlider->setValue(m_sink->volume() * 100);
    speakerForm->addWidget(outputVolumeSlider, 0, 1, Qt::AlignVCenter);

    connect(outputVolumeSlider, &DSlider::valueChanged, [=](int value){
        m_sink->SetVolume(value / 100.0, true);
    });
    connect(m_sink, &DBusAudioSink::VolumeChanged, [=]{
        if (qAbs(m_sink->volume() * 100 - outputVolumeSlider->value()) > 1) {
            outputVolumeSlider->setValue(m_sink->volume() * 100);
        }
    });

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
    inputVolumeSlider->setRange(0, 150);
    inputVolumeSlider->setValue(m_source->volume() * 100);
    microphoneForm->addWidget(inputVolumeSlider, 0, 1, Qt::AlignVCenter);

    connect(inputVolumeSlider, &DSlider::valueChanged, [=](int value){
        m_source->SetVolume(value / 100.0, true);
    });
    connect(m_source, &DBusAudioSource::VolumeChanged, [=]{
        if (qAbs(m_source->volume() * 100 - inputVolumeSlider->value()) > 1) {
            inputVolumeSlider->setValue(m_source->volume() * 100);
        }
    });

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
    outputPortsExpand->setExpand(true);

    HeaderLine * outputPortsLine = new HeaderLine("Output ports", outputPortsExpand);
    outputPortsExpand->setHeader(outputPortsLine);

    SinkPortStruct sinkActivePort = m_sink->activePort();
    SinkPortList sinkPorts = m_sink->ports();

    QStringList outputPorts;
    foreach (SinkPortStruct port, sinkPorts) {
        outputPorts << port.name;
    }

    DButtonList * outputPortsList = new DButtonList(outputPortsExpand);
    outputPortsList->addButtons(outputPorts);
    outputPortsList->checkButtonByIndex(sinkActivePort.index);
    outputPortsList->setFixedSize(310, outputPorts.length() * outputPortsList->itemWidget(outputPortsList->item(0))->height());

    outputPortsExpand->setContent(outputPortsList);

    mainLayout->addWidget(outputPortsExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // Output devices
    DBaseExpand * outputDevicesExpand = new DBaseExpand;
    outputDevicesExpand->setExpand(true);

    HeaderLine * outputDevicesLine = new HeaderLine("Output devices", outputDevicesExpand);
    outputDevicesExpand->setHeader(outputDevicesLine);

    QStringList outputDevices;
    outputDevices << m_sink->description();
    DButtonList * outputDevicesList = new DButtonList(outputDevicesExpand);
    outputDevicesList->addButtons(outputDevices);
    outputDevicesList->setFixedSize(310, outputDevices.length() * outputDevicesList->itemWidget(outputDevicesList->item(0))->height());
    outputDevicesList->checkButtonByIndex(0);

    outputDevicesExpand->setContent(outputDevicesList);

    mainLayout->addWidget(outputDevicesExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(new DBaseLine);

    // Input ports
    DBaseExpand * inputPortsExpand = new DBaseExpand;
    inputPortsExpand->setExpand(true);

    HeaderLine * inputPortsLine = new HeaderLine("Input ports", inputPortsExpand);
    inputPortsExpand->setHeader(inputPortsLine);

    SourcePortStruct sourceActivePort = m_source->activePort();
    SourcePortList sourcePorts = m_source->ports();

    QStringList inputPorts;
    foreach (SourcePortStruct port, sourcePorts) {
        inputPorts << port.name;
    }

    DButtonList * inputPortsList = new DButtonList(inputPortsExpand);
    inputPortsList->addButtons(inputPorts);
    inputPortsList->setFixedSize(310, inputPorts.length() * inputPortsList->itemWidget(inputPortsList->item(0))->height());
    inputPortsList->checkButtonByIndex(sourceActivePort.index);

    inputPortsExpand->setContent(inputPortsList);

    mainLayout->addWidget(inputPortsExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // Input devices
    DBaseExpand * inputDevicesExpand = new DBaseExpand;
    inputDevicesExpand->setExpand(true);

    HeaderLine * inputDevicesLine = new HeaderLine("Input devices", inputDevicesExpand);
    inputDevicesExpand->setHeader(inputDevicesLine);

    QStringList inputDevices;
    inputDevices << m_source->description();

    DButtonList * inputDevicesList = new DButtonList(inputDevicesExpand);
    inputDevicesList->addButtons(inputDevices);
    inputDevicesList->setFixedSize(310, inputDevices.length() * inputDevicesList->itemWidget(inputDevicesList->item(0))->height());
    inputDevicesList->checkButtonByIndex(0);

    inputDevicesExpand->setContent(inputDevicesList);

    mainLayout->addWidget(inputDevicesExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    mainLayout->addStretch(1);

    // expand settings
    speakerExpand->setExpand(!m_sink->mute());
    connect(m_sink, &DBusAudioSink::MuteChanged, [=]{
        speakerExpand->setExpand(!m_sink->mute());
    });
    mircophoneExpand->setExpand(!m_source->mute());
    connect(m_source, &DBusAudioSource::MuteChanged, [=]{
       mircophoneExpand->setExpand(!m_source->mute());
    });
}

QFrame* Sound::getContent()
{
    qDebug() << "Sound module" << "getContent";
    return m_frame;
}
