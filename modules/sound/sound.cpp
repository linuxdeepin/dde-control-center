#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDBusObjectPath>

#include <libdui/dseparatorhorizontal.h>

#include "sound.h"
#include "moduleheader.h"
#include "headerline.h"
#include "normallabel.h"

DUI_USE_NAMESPACE

Sound::Sound() :
    QObject(),
    m_frame(new QFrame)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    initBackend();

    if (m_sink && m_source) {
        initUI();
    }
}

Sound::~Sound()
{
    qDebug() << "~Sound";
    delete m_frame;
}

void Sound::initBackend()
{
    m_dbusAudio = new DBusAudio(this);

    QList<QDBusObjectPath> sinkPaths = m_dbusAudio->sinks();
    foreach (QDBusObjectPath path, sinkPaths) {
        m_sinks << new DBusAudioSink(path.path(), this);
    }

    QList<QDBusObjectPath> sourcePaths = m_dbusAudio->sources();
    foreach (QDBusObjectPath path, sourcePaths) {
        m_sources << new DBusAudioSource(path.path(), this);
    }

    m_sink = getDefaultSink();
    m_source = getDefaultSource();

    QString meterPath = m_source->GetMeter().value().path();
    QString meterName = meterPath;
    meterName = meterName.replace("/", ".").mid(1);
    m_dbusMeter = new QDBusInterface("com.deepin.daemon.Audio", meterPath, meterName);
    connect(&m_meterTimer, &QTimer::timeout, [&]{
        m_dbusMeter->call("Tick");
    });
    QDBusConnection::sessionBus().connect(m_dbusMeter->service(), m_dbusMeter->path(), "org.freedesktop.DBus.Properties",  "PropertiesChanged","sa{sv}as", this, SLOT(meterVolumeChanged(QDBusMessage)));
    m_meterTimer.start(2000);
    m_dbusMeter->call("Tick");
}

void Sound::initUI()
{
    m_frame->setFixedWidth(310);

    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    ///////////////////////////////////////////////////////-- Title
    ModuleHeader * header = new ModuleHeader("Sound");
    mainLayout->addWidget(header);
    mainLayout->addWidget(new DSeparatorHorizontal);

    connect(header, &ModuleHeader::resetButtonClicked, [=]{
        m_dbusAudio->Reset();
    });

    ///////////////////////////////////////////////////////-- Speaker Settings
    m_speakerExpand = new DSwitchLineExpand;
    m_speakerExpand->setTitle("Speaker");

    QFrame * speakerExpandContent = new QFrame(m_speakerExpand);
    speakerExpandContent->setFixedWidth(310);
    speakerExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout * speakerForm = new QGridLayout(speakerExpandContent);
    speakerForm->setRowMinimumHeight(0, 36);
    speakerForm->setRowMinimumHeight(1, 36);

    // Output volume line
    speakerForm->addWidget(new NormalLabel("Output volume"), 0, 0, Qt::AlignVCenter);
    m_outputVolumeSlider = new DSlider(Qt::Horizontal);
    m_outputVolumeSlider->setTracking(false);
    m_outputVolumeSlider->setRange(0, 150);
    m_outputVolumeSlider->setLeftTip("-");
    m_outputVolumeSlider->setRightTip("+");
    m_outputVolumeSlider->addScale(100);
    speakerForm->addWidget(m_outputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // Left/Right balance line
    speakerForm->addWidget(new NormalLabel("Left/Right balance"), 1, 0, Qt::AlignVCenter);
    m_leftRightBalanceSlider = new DSlider(Qt::Horizontal);
    m_leftRightBalanceSlider->setHandleType(DSlider::SharpHandler);
    m_leftRightBalanceSlider->setRange(-100, 100);
    m_leftRightBalanceSlider->setLeftTip(tr("Left"));
    m_leftRightBalanceSlider->setRightTip(tr("Right"));
    m_leftRightBalanceSlider->addScale(0);
    speakerForm->addWidget(m_leftRightBalanceSlider, 1, 1, Qt::AlignVCenter);

    updateSpeakerUI();
    speakerExpandContent->adjustSize();
    speakerExpandContent->setFixedHeight(speakerExpandContent->height());

    m_speakerExpand->setContent(speakerExpandContent);
    mainLayout->addWidget(m_speakerExpand);
    mainLayout->addWidget(new DBaseLine);

    ///////////////////////////////////////////////////////-- Microphone Settings
    m_microphoneExpand = new DSwitchLineExpand;
    m_microphoneExpand->setTitle("Microphone");

    QFrame * mircophoneExpandContent = new QFrame(m_microphoneExpand);
    mircophoneExpandContent->setFixedWidth(310);
    mircophoneExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout * microphoneForm = new QGridLayout(mircophoneExpandContent);
    microphoneForm->setRowMinimumHeight(0, 36);
    microphoneForm->setRowMinimumHeight(1, 36);

    // microphone volume line
    microphoneForm->addWidget(new NormalLabel("Input volume"), 0, 0, Qt::AlignVCenter);
    m_inputVolumeSlider = new DSlider(Qt::Horizontal);
    m_inputVolumeSlider->setRange(0, 150);
    m_inputVolumeSlider->setLeftTip("-");
    m_inputVolumeSlider->setRightTip("+");
    m_inputVolumeSlider->addScale(100);
    microphoneForm->addWidget(m_inputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // microphone feedback line
    microphoneForm->addWidget(new NormalLabel("Feedback volume"), 1, 0, Qt::AlignVCenter);
    m_inputFeedbackSlider = new DSlider(Qt::Horizontal);
    QString feedbackSliderStyle = m_inputFeedbackSlider->styleSheet();
    feedbackSliderStyle += "DUI--DSlider::handle[handleType=\"1\"] {background: none;}DUI--DSlider::add-page:horizontal[handleType=\"1\"]{border-width: 0px 2px 1px 0px;}DUI--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}";
    m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle);
    m_inputFeedbackSlider->setRange(0, 100);
    connect(m_inputFeedbackSlider, &DSlider::valueChanged, [=](int value){
        if(value > 80){
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
            +"DUI--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0a73bb, stop:0.66 #0a73bb, stop:0.76 #ffbf0f, stop:1 #ff8503);}");
        }else{
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
            +"DUI--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}");
        }
    });
    microphoneForm->addWidget(m_inputFeedbackSlider, 1, 1, Qt::AlignVCenter);

    updateMicrophoneUI();
    mircophoneExpandContent->setFixedHeight(speakerExpandContent->height());
    mircophoneExpandContent->adjustSize();

    m_microphoneExpand->setContent(mircophoneExpandContent);
    mainLayout->addWidget(m_microphoneExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(new DBaseLine);

    ///////////////////////////////////////////////////////--Advanced settings
    // Output ports
    m_outputPortsExpand = new DBaseExpand;
    m_outputPortsExpand->setExpand(true);

    HeaderLine * outputPortsLine = new HeaderLine("Output ports", m_outputPortsExpand);
    m_outputPortsExpand->setHeader(outputPortsLine);

    m_outputPortsList = new DButtonList(m_outputPortsExpand);
    m_outputPortsExpand->setContent(m_outputPortsList);
    updateOutputPorts();

    mainLayout->addWidget(m_outputPortsExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // Output devices
    DBaseExpand * outputDevicesExpand = new DBaseExpand;
    outputDevicesExpand->setExpand(true);

    HeaderLine * outputDevicesLine = new HeaderLine("Output devices", outputDevicesExpand);
    outputDevicesExpand->setHeader(outputDevicesLine);

    QStringList outputDevices;
    foreach (DBusAudioSink * sink, m_sinks) {
        outputDevices << sink->description();
    }

    DButtonList * outputDevicesList = new DButtonList(outputDevicesExpand);
    outputDevicesList->addButtons(outputDevices);
    outputDevicesList->setFixedSize(310, outputDevices.length() * outputDevicesList->itemWidget(outputDevicesList->item(0))->height());
    outputDevicesList->checkButtonByIndex(m_sinks.indexOf(m_sink));

    connect(outputDevicesList, &DButtonList::buttonCheckedIndexChanged, [=](int index) {
        DBusAudioSink * sink = m_sinks.at(index);
        m_dbusAudio->SetDefaultSink(sink->name());
    });
    connect(m_dbusAudio, &DBusAudio::DefaultSinkChanged, [=]{
        m_sink = getDefaultSink();
        outputDevicesList->checkButtonByIndex(m_sinks.indexOf(m_sink));

        updateSpeakerUI();
    });

    outputDevicesExpand->setContent(outputDevicesList);

    mainLayout->addWidget(outputDevicesExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(new DBaseLine);

    // Input ports
    m_inputPortsExpand = new DBaseExpand;
    m_inputPortsExpand->setExpand(true);

    HeaderLine * inputPortsLine = new HeaderLine("Input ports", m_inputPortsExpand);
    m_inputPortsExpand->setHeader(inputPortsLine);

    m_inputPortsList = new DButtonList(m_inputPortsExpand);
    m_inputPortsExpand->setContent(m_inputPortsList);
    updateInputPorts();

    mainLayout->addWidget(m_inputPortsExpand);
    mainLayout->addWidget(new DSeparatorHorizontal);

    // Input devices
    DBaseExpand * inputDevicesExpand = new DBaseExpand;
    inputDevicesExpand->setExpand(true);

    HeaderLine * inputDevicesLine = new HeaderLine("Input devices", inputDevicesExpand);
    inputDevicesExpand->setHeader(inputDevicesLine);

    QStringList inputDevices;
    foreach (DBusAudioSource * source, m_sources) {
        inputDevices << source->description();
    }

    DButtonList * inputDevicesList = new DButtonList(inputDevicesExpand);
    inputDevicesList->addButtons(inputDevices);
    inputDevicesList->setFixedSize(310, inputDevices.length() * inputDevicesList->itemWidget(inputDevicesList->item(0))->height());
    inputDevicesList->checkButtonByIndex(m_sources.indexOf(m_source));

    connect(inputDevicesList, &DButtonList::buttonCheckedIndexChanged, [=](int index) {
        DBusAudioSource * source = m_sources.at(index);
        m_dbusAudio->SetDefaultSource(source->name());
    });
    connect(m_dbusAudio, &DBusAudio::DefaultSourceChanged, [=]{
        m_source = getDefaultSource();
        inputDevicesList->checkButtonByIndex(m_sources.indexOf(m_source));
    });

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

DBusAudioSink * Sound::getDefaultSink()
{
    foreach (DBusAudioSink * s, m_sinks) {
        if (s->name() == m_dbusAudio->defaultSink()) {
            return s;
        }
    }

    return NULL;
}

DBusAudioSource * Sound::getDefaultSource()
{
    foreach (DBusAudioSource * s, m_sources) {
        if (s->name() == m_dbusAudio->defaultSource()) {
            return s;
        }
    }

    return NULL;
}

void Sound::updateSpeakerUI()
{
    m_outputVolumeSlider->setValue(m_sink->volume() * 100);
    connect(m_outputVolumeSlider, &DSlider::valueChanged, [=](int value){
        m_sink->SetVolume(value / 100.0, true);
    });
    connect(m_sink, &DBusAudioSink::VolumeChanged, [=]{
        if (qAbs(m_sink->volume() * 100 - m_outputVolumeSlider->value()) > 1) {
            m_outputVolumeSlider->setValue(m_sink->volume() * 100);
        }
    });

    m_leftRightBalanceSlider->setValue(m_sink->balance() * 100);
    connect(m_leftRightBalanceSlider, &DSlider::valueChanged, [=](int value){
        m_sink->SetBalance(value / 100.0, true);
    });
    connect(m_sink, &DBusAudioSink::BalanceChanged, [=]{
        if(qAbs(m_sink->balance() * 100 - m_leftRightBalanceSlider->value()) > 1) {
            m_leftRightBalanceSlider->setValue(m_sink->balance() * 100);
        }
    });

    m_speakerExpand->setExpand(!m_sink->mute());
    connect(m_sink, &DBusAudioSink::MuteChanged, [=]{
        m_speakerExpand->setExpand(!m_sink->mute());
    });
    connect(m_speakerExpand, &DBaseExpand::expandChange, [=] (bool expanded) {
        m_sink->SetMute(!expanded);
    });
}

void Sound::updateMicrophoneUI()
{
    m_inputVolumeSlider->setValue(m_source->volume() * 100);
    connect(m_inputVolumeSlider, &DSlider::valueChanged, [=](int value){
        m_source->SetVolume(value / 100.0, true);
    });
    connect(m_source, &DBusAudioSource::VolumeChanged, [=]{
        if (qAbs(m_source->volume() * 100 - m_inputVolumeSlider->value()) > 1) {
            m_inputVolumeSlider->setValue(m_source->volume() * 100);
        }
    });

    m_microphoneExpand->setExpand(!m_source->mute());
    connect(m_source, &DBusAudioSource::MuteChanged, [=]{
        m_microphoneExpand->setExpand(!m_source->mute());
    });
    connect(m_microphoneExpand, &DBaseExpand::expandChange, [=](bool expanded){
        m_source->SetMute(!expanded);
    });
}

void Sound::updateOutputPorts()
{
    SinkPortStruct sinkActivePort = m_sink->activePort();
    SinkPortList sinkPorts = m_sink->ports();

    QStringList outputPorts;
    foreach (SinkPortStruct port, sinkPorts) {
        outputPorts << port.name;
    }

    if (outputPorts.length() > 0) {
        m_outputPortsList->addButtons(outputPorts);
        m_outputPortsList->checkButtonByIndex(outputPorts.indexOf(sinkActivePort.name));
        m_outputPortsList->setFixedSize(310, outputPorts.length() * m_outputPortsList->itemWidget(m_outputPortsList->item(0))->height());

        m_outputPortsExpand->updateContentHeight();

        connect(m_outputPortsList, &DButtonList::buttonCheckedIndexChanged, [=](int index){
            SinkPortStruct port = sinkPorts.at(index);
            m_sink->SetPort(port.id);
        });
        connect(m_sink, &DBusAudioSink::ActivePortChanged, [=]{
            SinkPortStruct activePort = m_sink->activePort();
            m_outputPortsList->checkButtonByIndex(outputPorts.indexOf(activePort.name));
        });
    }
}

void Sound::updateInputPorts()
{
    SourcePortStruct sourceActivePort = m_source->activePort();
    SourcePortList sourcePorts = m_source->ports();

    QStringList inputPorts;
    foreach (SourcePortStruct port, sourcePorts) {
        inputPorts << port.name;
    }

    if (inputPorts.length() > 0) {
        m_inputPortsList->addButtons(inputPorts);
        m_inputPortsList->checkButtonByIndex(inputPorts.indexOf(sourceActivePort.name));
        m_inputPortsList->setFixedSize(310, inputPorts.length() * m_inputPortsList->itemWidget(m_inputPortsList->item(0))->height());

        m_inputPortsExpand->updateContentHeight();

        connect(m_inputPortsList, &DButtonList::buttonCheckedIndexChanged, [=](int index){
            SourcePortStruct port = sourcePorts.at(index);
            m_source->SetPort(port.id);
        });
        connect(m_source, &DBusAudioSource::ActivePortChanged, [=]{
            SourcePortStruct activePort = m_source->activePort();
            m_inputPortsList->checkButtonByIndex(inputPorts.indexOf(activePort.name));

            updateMicrophoneUI();
        });
    }
}

void Sound::meterVolumeChanged(const QDBusMessage &msg)
{
    QList<QVariant> arguments = msg.arguments();

    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    QStringList keys = changedProps.keys();
    foreach(const QString &prop, keys) {
        if(prop == "Volume"){
            m_inputFeedbackSlider->setValue(changedProps[prop].toDouble() * 100);
        }
    }
}
