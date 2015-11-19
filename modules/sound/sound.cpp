#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDBusObjectPath>
#include <QGuiApplication>
#include <QScreen>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dlinkbutton.h>

#include "constants.h"

#include "sound.h"
#include "moduleheader.h"
#include "headerline.h"
#include "normallabel.h"

DUI_USE_NAMESPACE

Sound::Sound() :
    QObject(),
    m_frame(new QFrame),
    m_inputFeedbackSlider(NULL),
    m_mainWidget(new QFrame)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Sound"));

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
    m_mainWidget->deleteLater();
}

void Sound::initBackend()
{
    m_dbusAudio = new DBusAudio(this);

    updateSinks();
    updateSources();
}

void Sound::initUI()
{
    QVBoxLayout *mainWidgetVLayout = new QVBoxLayout(m_mainWidget);
    mainWidgetVLayout->setSpacing(0);
    mainWidgetVLayout->setMargin(0);

    m_frame->setFixedWidth(DCC::ModuleContentWidth);
    m_frame->setObjectName("SoundMainFrame");
    m_frame->setStyleSheet("QFrame#SoundMainFrame{background:transparent}");

    m_scrollArea = new DScrollArea;
    m_scrollArea->setFixedSize(DCC::ModuleContentWidth, qApp->primaryScreen()->geometry().height() - 60);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_frame);

    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    ///////////////////////////////////////////////////////-- Title
    ModuleHeader * header = new ModuleHeader(tr("Sound"), true, m_mainWidget);
    mainWidgetVLayout->addWidget(header);
    mainWidgetVLayout->addWidget(new DSeparatorHorizontal);
    mainWidgetVLayout->addWidget(m_scrollArea);
    mainWidgetVLayout->addStretch(1);

    connect(header, &ModuleHeader::resetButtonClicked, m_dbusAudio, &DBusAudio::Reset);

    ///////////////////////////////////////////////////////-- Speaker Settings
    m_speakerExpand = new DSwitchLineExpand;
    m_speakerExpand->setTitle(tr("Speaker"));

    QFrame * speakerExpandContent = new QFrame(m_speakerExpand);
    speakerExpandContent->setFixedWidth(DCC::ModuleContentWidth);
    speakerExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout * speakerForm = new QGridLayout(speakerExpandContent);
    speakerForm->setRowMinimumHeight(0, 36);
    speakerForm->setRowMinimumHeight(1, 36);

    // Output volume line
    speakerForm->addWidget(new NormalLabel(tr("Output Volume")), 0, 0, Qt::AlignVCenter);
    m_outputVolumeSlider = new DSlider(Qt::Horizontal);
    m_outputVolumeSlider->setTracking(false);
    m_outputVolumeSlider->setRange(0, 150);
    m_outputVolumeSlider->setLeftTip("-");
    m_outputVolumeSlider->setRightTip("+");
    m_outputVolumeSlider->addScale(100);
    m_outputVolumeSlider->setHoverShowValue(true);
    m_outputVolumeSlider->setHoverShowValueInterval(1000);
    speakerForm->addWidget(m_outputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // Left/Right balance line
    speakerForm->addWidget(new NormalLabel(tr("Left/Right Balance")), 1, 0, Qt::AlignVCenter);
    m_leftRightBalanceSlider = new DSlider(Qt::Horizontal);
    m_leftRightBalanceSlider->setHandleType(DSlider::SharpHandler);
    m_leftRightBalanceSlider->setRange(-100, 100);
    m_leftRightBalanceSlider->setLeftTip(tr("Left"));
    m_leftRightBalanceSlider->setRightTip(tr("Right"));
    m_leftRightBalanceSlider->addScale(0);
    m_leftRightBalanceSlider->setHoverShowValue(true);
    m_leftRightBalanceSlider->setHoverShowValueInterval(1000);
    speakerForm->addWidget(m_leftRightBalanceSlider, 1, 1, Qt::AlignVCenter);

    updateSpeakerUI();
    speakerExpandContent->adjustSize();
    speakerExpandContent->setFixedHeight(speakerExpandContent->height());

    m_speakerExpand->setContent(speakerExpandContent);
    mainLayout->addWidget(m_speakerExpand);
    mainLayout->addWidget(new DBaseLine);

    ///////////////////////////////////////////////////////-- Microphone Settings
    m_microphoneExpand = new DSwitchLineExpand;
    m_microphoneExpand->setTitle(tr("Microphone"));

    QFrame * mircophoneExpandContent = new QFrame(m_microphoneExpand);
    mircophoneExpandContent->setFixedWidth(DCC::ModuleContentWidth);
    mircophoneExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout * microphoneForm = new QGridLayout(mircophoneExpandContent);
    microphoneForm->setRowMinimumHeight(0, 36);
    microphoneForm->setRowMinimumHeight(1, 36);

    // microphone volume line
    microphoneForm->addWidget(new NormalLabel(tr("Input Volume")), 0, 0, Qt::AlignVCenter);
    m_inputVolumeSlider = new DSlider(Qt::Horizontal);
    m_inputVolumeSlider->setRange(0, 150);    qDebug() << "Sound module" << "getContent" << m_mainWidget;
    m_inputVolumeSlider->setLeftTip("-");
    m_inputVolumeSlider->setRightTip("+");
    m_inputVolumeSlider->addScale(100);
    m_inputVolumeSlider->setHoverShowValue(true);
    m_inputVolumeSlider->setHoverShowValueInterval(1000);
    microphoneForm->addWidget(m_inputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // microphone feedback line
    microphoneForm->addWidget(new NormalLabel(tr("Feedback Volume")), 1, 0, Qt::AlignVCenter);
    m_inputFeedbackSlider = new DSlider(Qt::Horizontal);
    QString feedbackSliderStyle = m_inputFeedbackSlider->styleSheet();
    feedbackSliderStyle += "DUI--DSlider::handle[handleType=\"1\"] {background: none;}DUI--DSlider::add-page:horizontal[handleType=\"1\"]{border-width: 0px 2px 1px 0px;}";
    m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle);
    m_inputFeedbackSlider->setRange(0, 100);
    m_inputFeedbackSlider->setEnabled(false);
    connect(m_inputFeedbackSlider, &DSlider::valueChanged, [=](int value){
        if(value > 80){
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
            +"DUI--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0a73bb, stop:0.66 #0a73bb, stop:0.76 #ffbf0f, stop:1 #ff8503);}DUI--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}");
        }else if(value < 3){
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
            +"DUI--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}DUI--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 2px;}");
        }else{
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
            +"DUI--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}DUI--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}");
        }
    });
    microphoneForm->addWidget(m_inputFeedbackSlider, 1, 1, Qt::AlignVCenter);

    updateMicrophoneUI();
    mircophoneExpandContent->setFixedHeight(speakerExpandContent->height());
    mircophoneExpandContent->adjustSize();

    m_microphoneExpand->setContent(mircophoneExpandContent);
    mainLayout->addWidget(m_microphoneExpand);

    ///advanced settings expand widget

    QWidget *advanced_widget = new QWidget;
    advanced_widget->setFixedSize(DCC::ModuleContentWidth, 400);
    ///400 is advanced settings height sum.

    QVBoxLayout *advanced_layout = new QVBoxLayout(advanced_widget);
    advanced_layout->setMargin(0);
    advanced_layout->setSpacing(0);

    DBaseExpand *advanced_expand = new DBaseExpand;
    advanced_expand->setContent(advanced_widget);
    advanced_expand->setSeparatorVisible(false);
    advanced_expand->setExpandedSeparatorVisible(false);

    advanced_layout->addWidget(new DBaseLine);

    ///////////////////////////////////////////////////////--Advanced settings
    // Output ports
    m_outputPortsExpand = new DBaseExpand;
    m_outputPortsExpand->setExpand(true);

    HeaderLine * outputPortsLine = new HeaderLine(tr("Output Port"), m_outputPortsExpand);
    m_outputPortsExpand->setHeader(outputPortsLine);

    m_outputPortsList = new DButtonList(m_outputPortsExpand);
    m_outputPortsExpand->setContent(m_outputPortsList);
    updateOutputPorts();

    advanced_layout->addWidget(m_outputPortsExpand);

    // Output devices
    m_outputDevicesExpand = new DBaseExpand;
    m_outputDevicesExpand->setExpand(true);

    HeaderLine * outputDevicesLine = new HeaderLine(tr("Output device"), m_outputDevicesExpand);
    m_outputDevicesExpand->setHeader(outputDevicesLine);

    m_outputDevicesList = new DButtonList(m_outputDevicesExpand);
    m_outputDevicesExpand->setContent(m_outputDevicesList);
    updateOutputDevices();
    connect(m_dbusAudio, &DBusAudio::SinksChanged, this, &Sound::updateOutputDevices);

    advanced_layout->addWidget(m_outputDevicesExpand);
    advanced_layout->addWidget(new DBaseLine);

    // Input ports
    m_inputPortsExpand = new DBaseExpand;
    m_inputPortsExpand->setExpand(true);

    HeaderLine * inputPortsLine = new HeaderLine(tr("Input Port"), m_inputPortsExpand);
    m_inputPortsExpand->setHeader(inputPortsLine);

    m_inputPortsList = new DButtonList(m_inputPortsExpand);
    m_inputPortsExpand->setContent(m_inputPortsList);
    updateInputPorts();

    advanced_layout->addWidget(m_inputPortsExpand);

    // Input devices
    m_inputDevicesExpand = new DBaseExpand;
    m_inputDevicesExpand->setExpandedSeparatorVisible(false);
    m_inputDevicesExpand->setExpand(true);

    HeaderLine * inputDevicesLine = new HeaderLine(tr("Input device"), m_inputDevicesExpand);
    m_inputDevicesExpand->setHeader(inputDevicesLine);

    m_inputDevicesList = new DButtonList(m_inputDevicesExpand);
    m_inputDevicesExpand->setContent(m_inputDevicesList);
    updateInputDevices();
    connect(m_dbusAudio, &DBusAudio::SourcesChanged, this, &Sound::updateInputDevices);

    // show advanced
    DLinkButton *advanced_button = new DLinkButton(tr("Show Advanced...") + "      ");
    advanced_button->setStyleSheet(advanced_button->styleSheet() + "DUI--DLinkButton:pressed{color:#0188FF;}");
    connect(advanced_button, &DLinkButton::clicked, [this, advanced_button, advanced_expand]{
        if(advanced_expand->expand()){
            m_frame->setFixedHeight(150);
            ///When the height of the sum of the hide when advanced settings
            advanced_expand->setExpand(false);
            advanced_button->setText(tr("Show Advanced...") + "      ");
        }else{
            m_soundEffectsExpand->setExpand(false);
            m_frame->setFixedHeight(650);
            ///When the height of the sum of the show when advanced settings
            advanced_expand->setExpand(true);
            advanced_button->setText(tr("Hide Advanced...") + "      ");
        }
    });

    m_soundEffectsInter = new DBusSoundEffects("com.deepin.daemon.SoundEffect",
                                               "/com/deepin/daemon/SoundEffect",
                                               QDBusConnection::sessionBus(), this);

    m_loginSoundSwitch = new SoundEffectSwitchWidget;
    m_loginSoundSwitch->setSwitched(m_soundEffectsInter->login());
    m_loginSoundSwitch->setTitle(tr("Login"));
    m_loginSoundSwitch->setFixedWidth(DCC::ModuleContentWidth);
//    m_logoutSoundSwitch = new SoundEffectSwitchWidget;
//    m_logoutSoundSwitch->setSwitched(m_soundEffectsInter->logout());
//    m_logoutSoundSwitch->setTitle(tr("Logout"));
//    m_shutdownSoundSwitch = new SoundEffectSwitchWidget;
//    m_shutdownSoundSwitch->setSwitched(m_soundEffectsInter->shutdown());
//    m_shutdownSoundSwitch->setTitle(tr("Shutdown"));
    m_restoreSoundSwitch = new SoundEffectSwitchWidget;
    m_restoreSoundSwitch->setSwitched(m_soundEffectsInter->wakeup());
    m_restoreSoundSwitch->setTitle(tr("Restore"));

    QVBoxLayout *soundEffectsLayout = new QVBoxLayout;
    soundEffectsLayout->addWidget(m_loginSoundSwitch);
//    soundEffectsLayout->addWidget(m_logoutSoundSwitch);
//    soundEffectsLayout->addWidget(m_shutdownSoundSwitch);
    soundEffectsLayout->addWidget(m_restoreSoundSwitch);
    soundEffectsLayout->setSpacing(0);
    soundEffectsLayout->setMargin(0);

    QWidget *soundEffectsWidget = new QWidget;
    soundEffectsWidget->setLayout(soundEffectsLayout);
//    soundEffectsWidget->setFixedHeight(50 * 4);
    soundEffectsWidget->setFixedHeight(50 * 2);

    m_soundEffectsExpand = new DArrowLineExpand;
    m_soundEffectsExpand->setContent(soundEffectsWidget);
    m_soundEffectsExpand->setTitle(tr("Sound Effects"));
    m_soundEffectsExpand->setExpandedSeparatorVisible(false);

    connect(m_loginSoundSwitch, &SoundEffectSwitchWidget::switchToggled, [this] (bool e) {
        m_soundEffectsInter->setLogin(e);
    });
//    connect(m_logoutSoundSwitch, &SoundEffectSwitchWidget::switchToggled, [this] (bool e) {
//        m_soundEffectsInter->setLogout(e);
//    });
//    connect(m_shutdownSoundSwitch, &SoundEffectSwitchWidget::switchToggled, [this] (bool e) {
//        m_soundEffectsInter->setShutdown(e);
//    });
    connect(m_restoreSoundSwitch, &SoundEffectSwitchWidget::switchToggled, [this] (bool e) {
        m_soundEffectsInter->setWakeup(e);
    });
    connect(m_soundEffectsExpand, &DArrowLineExpand::expandChange, [advanced_expand] (bool e) {
        if (e)
            advanced_expand->setExpand(false);
    });

    advanced_layout->addWidget(m_inputDevicesExpand);
    advanced_layout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(advanced_expand);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(advanced_button, 0, Qt::AlignRight);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(m_soundEffectsExpand);
    mainLayout->addStretch(1);
}

QFrame* Sound::getContent()
{
    return m_mainWidget;
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

void Sound::updateSinks()
{
    qDebug() << "updateSinks";
    m_sinks.clear();

    QList<QDBusObjectPath> sinkPaths = m_dbusAudio->sinks();
    foreach (QDBusObjectPath path, sinkPaths) {
        m_sinks << new DBusAudioSink(path.path(), this);
    }

    m_sink = getDefaultSink();
}

void Sound::updateSources()
{
    qDebug() << "updateSources";

    m_sources.clear();

    QList<QDBusObjectPath> sourcePaths = m_dbusAudio->sources();
    foreach (QDBusObjectPath path, sourcePaths) {
        m_sources << new DBusAudioSource(path.path(), this);
    }

    m_source = getDefaultSource();

    // init meter
    QString meterPath = m_source->GetMeter().value().path();
    QString meterName = meterPath;
    meterName = meterName.replace("/", ".").mid(1);
    m_dbusMeter = new QDBusInterface("com.deepin.daemon.Audio", meterPath, meterName);
    m_dbusMeter->setParent(this);
    connect(&m_meterTimer, &QTimer::timeout, [&]{
        m_dbusMeter->call("Tick");
    });
    QDBusConnection::sessionBus().connect(m_dbusMeter->service(), m_dbusMeter->path(), "org.freedesktop.DBus.Properties",  "PropertiesChanged","sa{sv}as", this, SLOT(meterVolumeChanged(QDBusMessage)));
    m_meterTimer.start(2000);
    m_dbusMeter->call("Tick");
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
    qDebug() << "updateOutputPorts";

    SinkPortStruct sinkActivePort = m_sink->activePort();
    SinkPortList sinkPorts = m_sink->ports();

    QStringList outputPorts;
    foreach (SinkPortStruct port, sinkPorts) {
        outputPorts << port.name;
    }

    if (outputPorts.length() > 0) {
        m_outputPortsList->addButtons(outputPorts);
        m_outputPortsList->setFixedSize(DCC::ModuleContentWidth, outputPorts.length() * m_outputPortsList->itemWidget(m_outputPortsList->item(0))->height());
        if (outputPorts.contains(sinkActivePort.name)) {
            m_outputPortsList->checkButtonByIndex(outputPorts.indexOf(sinkActivePort.name));
        }

        connect(m_outputPortsList, &DButtonList::buttonCheckedIndexChanged, [=](int index){
            SinkPortStruct port = sinkPorts.at(index);
            m_sink->SetPort(port.id);
        });
        connect(m_sink, &DBusAudioSink::ActivePortChanged, [=]{
            SinkPortStruct activePort = m_sink->activePort();
            if (outputPorts.contains(activePort.name)) {
                m_outputPortsList->checkButtonByIndex(outputPorts.indexOf(activePort.name));
            }

            updateSpeakerUI();
        });
    }
}

void Sound::updateInputPorts()
{
    qDebug() << "updateInputPorts";

    SourcePortStruct sourceActivePort = m_source->activePort();
    SourcePortList sourcePorts = m_source->ports();

    QStringList inputPorts;
    foreach (SourcePortStruct port, sourcePorts) {
        inputPorts << port.name;
    }

    if (inputPorts.length() > 0) {
        m_inputPortsList->addButtons(inputPorts);
        m_inputPortsList->setFixedSize(DCC::ModuleContentWidth, inputPorts.length() * m_inputPortsList->itemWidget(m_inputPortsList->item(0))->height());
        if (inputPorts.contains(sourceActivePort.name)) {
            m_inputPortsList->checkButtonByIndex(inputPorts.indexOf(sourceActivePort.name));
        }

        connect(m_inputPortsList, &DButtonList::buttonCheckedIndexChanged, [=](int index){
            SourcePortStruct port = sourcePorts.at(index);
            m_source->SetPort(port.id);
        });
        connect(m_source, &DBusAudioSource::ActivePortChanged, [=]{
            SourcePortStruct activePort = m_source->activePort();
            if (inputPorts.contains(activePort.name)) {
                m_inputPortsList->checkButtonByIndex(inputPorts.indexOf(activePort.name));
            }

            updateMicrophoneUI();
        });
    }
}

void Sound::updateOutputDevices()
{
    qDebug() << "updateOutputDevices";

    updateSinks();

    QStringList outputDevices;
    foreach (DBusAudioSink * sink, m_sinks) {
        outputDevices << sink->description();
    }

    if (outputDevices.length() > 0) {
        m_outputDevicesList->clear();
        m_outputDevicesList->addButtons(outputDevices);
        m_outputDevicesList->setFixedSize(DCC::ModuleContentWidth, outputDevices.length() * m_outputDevicesList->itemWidget(m_outputDevicesList->item(0))->height());

        if (m_sinks.contains(m_sink)) {
            m_outputDevicesList->checkButtonByIndex(m_sinks.indexOf(m_sink));
        }

        connect(m_outputDevicesList, &DButtonList::buttonCheckedIndexChanged, [=](int index) {
            DBusAudioSink * sink = m_sinks.at(index);
            // important, otherwise there will be bounding loop.
            if (sink->name() != getDefaultSink()->name()) {
                m_dbusAudio->SetDefaultSink(sink->name());
            }
        });
        connect(m_dbusAudio, &DBusAudio::DefaultSinkChanged, [=]{
            m_sink = getDefaultSink();
            if (m_sinks.contains(m_sink)) {
                m_outputDevicesList->checkButtonByIndex(m_sinks.indexOf(m_sink));
            }

            updateOutputPorts();
        });
    }
}

void Sound::updateInputDevices()
{
    qDebug() << "updateInputDevices";

    updateSources();

    QStringList inputDevices;
    foreach (DBusAudioSource * source, m_sources) {
        inputDevices << source->description();
    }

    if (inputDevices.length() > 0) {
        m_inputDevicesList->clear();
        m_inputDevicesList->addButtons(inputDevices);
        m_inputDevicesList->setFixedSize(DCC::ModuleContentWidth, inputDevices.length() * m_inputDevicesList->itemWidget(m_inputDevicesList->item(0))->height());
        if (m_sources.contains(m_source)) {
            m_inputDevicesList->checkButtonByIndex(m_sources.indexOf(m_source));
        }

        connect(m_inputDevicesList, &DButtonList::buttonCheckedIndexChanged, [=](int index) {
            DBusAudioSource * source = m_sources.at(index);
            if (source->name() != getDefaultSource()->name()) {
                m_dbusAudio->SetDefaultSource(source->name());
            }
        });
        connect(m_dbusAudio, &DBusAudio::DefaultSourceChanged, [=]{
            m_source = getDefaultSource();
            if (m_sources.contains(m_source)) {
                m_inputDevicesList->checkButtonByIndex(m_sources.indexOf(m_source));
            }

            updateInputPorts();
        });
    }
}

void Sound::meterVolumeChanged(const QDBusMessage &msg)
{
    QList<QVariant> arguments = msg.arguments();

    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    QStringList keys = changedProps.keys();
    foreach(const QString &prop, keys) {
        if(prop == "Volume" && m_inputFeedbackSlider){
            m_inputFeedbackSlider->setValue(changedProps[prop].toDouble() * 100);
        }
    }
}
