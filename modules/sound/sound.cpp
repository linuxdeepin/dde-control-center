/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QFrame>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDBusObjectPath>
#include <QGuiApplication>
#include <QScreen>

#include <dseparatorhorizontal.h>
#include <dlinkbutton.h>
#include <dboxwidget.h>
#include <dswitchbutton.h>

#include "constants.h"

#include "sound.h"
#include "moduleheader.h"
#include "headerline.h"
#include "normallabel.h"

DWIDGET_USE_NAMESPACE

QFrame *SoundModule::getContent()
{
    static Sound *sound = NULL;
    if (!sound) {
        sound = new Sound;
    }
    return sound->getContent();
}

Sound::Sound() :
    QObject(),
    m_frame(new QFrame),
    m_inputFeedbackSlider(NULL),
    m_mainWidget(new QFrame),
    m_mainWidgetVLayout(new QVBoxLayout(m_mainWidget))
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Sound"));
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

//    m_delaySetOutputVolumeTimer.setSingleShot(true);
//    m_delaySetBalanceTimer.setSingleShot(true);

    // UI that can show immediately show first. # optimize for slow chips.
    m_mainWidgetVLayout->setSpacing(0);
    m_mainWidgetVLayout->setMargin(0);

    m_frame->setFixedWidth(DCC::ModuleContentWidth);
    m_frame->setObjectName("SoundMainFrame");
    m_frame->setStyleSheet("QFrame#SoundMainFrame{background:transparent}");

    m_scrollArea = new DScrollArea;
    m_scrollArea->setFixedSize(DCC::ModuleContentWidth, qApp->primaryScreen()->geometry().height() - 60);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_frame);

    ///////////////////////////////////////////////////////-- Title
    m_moduleHeader = new ModuleHeader(tr("Sound"), true, m_mainWidget);
    m_mainWidgetVLayout->addWidget(m_moduleHeader);
    m_mainWidgetVLayout->addWidget(new DSeparatorHorizontal);
    m_mainWidgetVLayout->addWidget(m_scrollArea);
    m_mainWidgetVLayout->addStretch(1);

#ifdef ARCH_MIPSEL
    QTimer::singleShot(100, [this]{
#endif
        initBackend();
        initUI();
#ifdef ARCH_MIPSEL
    });
#endif
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

    m_SetOutputVolumeRecorder.start();
    m_SetBalanceRecorder.start();
    m_SetInputVolumeRecorder.start();

    connect(m_moduleHeader, &ModuleHeader::resetButtonClicked, m_dbusAudio, &DBusAudio::Reset);
}

void Sound::initUI()
{
    QVBoxLayout * mainLayout = new QVBoxLayout(m_frame);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    if (m_sink) {
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
    }

    if (m_source) {
        ///////////////////////////////////////////////////////-- Microphone Settings
//        mainLayout->addWidget(new DBaseLine);
        m_microphoneExpand = new DSwitchLineExpand;
        m_microphoneExpand->setTitle(tr("Microphone"));

        QFrame * mircophoneExpandContent = new QFrame(m_microphoneExpand);
        mircophoneExpandContent->setFixedWidth(DCC::ModuleContentWidth);
        mircophoneExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        QGridLayout * microphoneForm = new QGridLayout(mircophoneExpandContent);
        microphoneForm->setRowMinimumHeight(0, 36);
#ifndef DCC_DISABLE_MICROPHONE_FEEDBACK
        microphoneForm->setRowMinimumHeight(1, 36);
#endif

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

#ifndef DCC_DISABLE_MICROPHONE_FEEDBACK
        // microphone feedback line
        microphoneForm->addWidget(new NormalLabel(tr("Feedback Volume")), 1, 0, Qt::AlignVCenter);
        m_inputFeedbackSlider = new DSlider(Qt::Horizontal);
        QString feedbackSliderStyle = m_inputFeedbackSlider->styleSheet();
        feedbackSliderStyle += "Dtk--Widget--DSlider::handle[handleType=\"1\"] {background: none;}Dtk--Widget--DSlider::add-page:horizontal[handleType=\"1\"]{border-width: 0px 2px 1px 0px;}";
        m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle);
        m_inputFeedbackSlider->setRange(0, 100);
        m_inputFeedbackSlider->setEnabled(false);
        connect(m_inputFeedbackSlider, &DSlider::valueChanged, [=](int value){
            // to prevent the slider being over refreshed.
            if (m_timeDeltaRecorder.elapsed() < 500) return;
            m_timeDeltaRecorder.restart();

            if(value > 80){
                m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
                                                     +"Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0a73bb, stop:0.66 #0a73bb, stop:0.76 #ffbf0f, stop:1 #ff8503);}Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}");
            }else if(value < 3){
                m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
                                                     +"Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 2px;}");
            }else{
                m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
                                                     +"Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}");
            }
        });
        microphoneForm->addWidget(m_inputFeedbackSlider, 1, 1, Qt::AlignVCenter);
#endif // DCC_DISABLE_MICROPHONE_FEEDBACK

        updateMicrophoneUI();
        //    mircophoneExpandContent->setFixedHeight(speakerExpandContent->height());
        mircophoneExpandContent->adjustSize();

        m_microphoneExpand->setContent(mircophoneExpandContent);
        mainLayout->addWidget(m_microphoneExpand);
    }

    ///advanced settings expand widget
    DVBoxWidget *advanced_widget = new DVBoxWidget;
//    advanced_widget->setFixedSize(DCC::ModuleContentWidth, 400);
    advanced_widget->setFixedWidth(DCC::ModuleContentWidth);
    ///400 is advanced settings height sum.

    QVBoxLayout *advanced_layout = new QVBoxLayout;
    advanced_layout->setMargin(0);
    advanced_layout->setSpacing(0);
//    advanced_widget->setLayout(advanced_layout);
    advanced_widget->layout()->addLayout(advanced_layout);

    DBaseExpand *advanced_expand = new DBaseExpand;
    advanced_expand->setContent(advanced_widget);
    advanced_expand->setSeparatorVisible(false);
    advanced_expand->setExpandedSeparatorVisible(false);

    DBaseLine *space = new DBaseLine;
    space->setStyleSheet("background-color:#252627;");
    advanced_layout->addWidget(space);

    ///////////////////////////////////////////////////////--Advanced settings
    if (m_sink) {
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
    }

    if (m_source) {
        DBaseLine *space1 = new DBaseLine;
        space1->setStyleSheet("background-color:#252627;");
        advanced_layout->addWidget(space1);

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

        advanced_layout->addWidget(m_inputDevicesExpand);
        advanced_layout->addWidget(new DSeparatorHorizontal);
    }

    // show advanced
    DLinkButton *advanced_button = new DLinkButton(tr("Show Advanced...") + "      ");
    advanced_button->setStyleSheet(advanced_button->styleSheet() + "Dtk--Widget--DLinkButton:pressed{color:#0188FF;}");
    connect(advanced_button, &DLinkButton::clicked, [this, advanced_button, advanced_expand]{
        if(advanced_expand->expand()){
            m_frame->setFixedHeight(150);
            ///When the height of the sum of the hide when advanced settings
            advanced_expand->setExpand(false);
            advanced_button->setText(tr("Show Advanced...") + "      ");
        }else{
            m_frame->setFixedHeight(650);
            ///When the height of the sum of the show when advanced settings
            advanced_expand->setExpand(true);
            advanced_button->setText(tr("Hide Advanced...") + "      ");
        }
    });

    m_soundEffectsInter = new DBusSoundEffects("com.deepin.daemon.SoundEffect",
                                               "/com/deepin/daemon/SoundEffect",
                                               QDBusConnection::sessionBus(), this);

    DSwitchButton * soundEffectsSwitch = new DSwitchButton;

    m_soundEffectsLine = new DHeaderLine;
    m_soundEffectsLine->setTitle(tr("Sound effects"));
    m_soundEffectsLine->setContent(soundEffectsSwitch);
    soundEffectsSwitch->setChecked(m_soundEffectsInter->enabled());

    connect(soundEffectsSwitch, &DSwitchButton::checkedChanged, [this, soundEffectsSwitch] {
        m_soundEffectsInter->setEnabled(soundEffectsSwitch->checked());
    });
    connect(m_soundEffectsInter, &DBusSoundEffects::EnabledChanged, [this, soundEffectsSwitch] {
        soundEffectsSwitch->setChecked(m_soundEffectsInter->enabled());
    });

    mainLayout->addWidget(m_soundEffectsLine);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(advanced_expand);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(advanced_button, 0, Qt::AlignRight);
    mainLayout->addSpacing(10);
    mainLayout->addStretch(1);

    if (m_sink) {
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

        connect(m_outputPortsList, &DButtonList::buttonCheckedIndexChanged, [=](int index){
            SinkPortStruct port =  m_sink->ports().at(index);
            m_sink->SetPort(port.id).waitForFinished();
        });

        connect(m_sink, &DBusAudioSink::ActivePortChanged, [=]{
            QStringList outputPorts;
            foreach (SinkPortStruct port, m_sink->ports()) {
                outputPorts << port.name;
            }

            SinkPortStruct activePort = m_sink->activePort();
            if (outputPorts.contains(activePort.name)) {
                m_outputPortsList->checkButtonByIndex(outputPorts.indexOf(activePort.name));
            }

            updateSpeakerUI();
        });

        connect(m_outputVolumeSlider, &DSlider::valueChanged, [=](int value){
            blockOutputVolumeSignal = true;
            int sliderInterval = m_SetOutputVolumeRecorder.elapsed();
            if (sliderInterval > 50) {
                if(qAbs(value - m_sink->volume() * 100) > 1) {
                    m_sink->SetVolume(value / 100.0, false);
                }
            }
            m_SetOutputVolumeRecorder.restart();

            // send the last change
            if(!m_SetOutputVolumeTimer) {
                m_SetOutputVolumeTimer = new QTimer(this);
                m_SetOutputVolumeTimer->setSingleShot(true);

                connect(m_SetOutputVolumeTimer, &QTimer::timeout, m_outputVolumeSlider, [this] {
                    m_SetOutputVolumeTimer->deleteLater();
                    m_SetOutputVolumeTimer = NULL;
                    int sliderValue = m_outputVolumeSlider->value();
                    if(qAbs(sliderValue - m_sink->volume() * 100) > 1) {
                        m_sink->SetVolume(sliderValue / 100.0, false);
                    }
                    blockOutputVolumeSignal = false;
                });
            }
            m_SetOutputVolumeTimer->start(200);
        });

        connect(m_sink, &DBusAudioSink::VolumeChanged, [=]{
            if (qAbs(m_sink->volume() * 100 - m_outputVolumeSlider->value()) > 1) {
                if (!blockOutputVolumeSignal) {
                    m_outputVolumeSlider->setValue(m_sink->volume() * 100);
                }
            }
        });

        connect(m_leftRightBalanceSlider, &DSlider::valueChanged, [=](int value){
            blockBalanceSignal = true;
            int sliderInterval = m_SetBalanceRecorder.elapsed();
            if (sliderInterval > 50) {
                if(qAbs(m_sink->balance() * 100 - value) > 1) {
                    m_sink->SetBalance(value / 100.0, false);
                }
            }
            m_SetBalanceRecorder.restart();

            // send the last change
            if(!m_SetBalanceTimer) {
                m_SetBalanceTimer = new QTimer(this);
                m_SetBalanceTimer->setSingleShot(true);

                connect(m_SetBalanceTimer, &QTimer::timeout, m_leftRightBalanceSlider, [this] {
                    m_SetBalanceTimer->deleteLater();
                    m_SetBalanceTimer = NULL;
                    int sliderValue = m_leftRightBalanceSlider->value();
                    if(qAbs(m_sink->balance() * 100 - sliderValue) > 1) {
                        m_sink->SetBalance(sliderValue / 100.0, false);
                    }
                    blockBalanceSignal = false;
                });
            }
            m_SetBalanceTimer->start(200);
        });
        connect(m_sink, &DBusAudioSink::BalanceChanged, [=]{
            if(qAbs(m_sink->balance() * 100 - m_leftRightBalanceSlider->value()) > 1) {
                if (!blockBalanceSignal) {
                    m_leftRightBalanceSlider->setValue(m_sink->balance() * 100);
                }
            }
        });

        connect(m_sink, &DBusAudioSink::MuteChanged, [=]{
            m_speakerExpand->setExpand(!m_sink->mute());
        });
        connect(m_speakerExpand, &DBaseExpand::expandChange, [=] (bool expanded) {
            m_sink->SetMute(!expanded).waitForFinished();
        });
    }

    if (m_source) {
        connect(m_inputVolumeSlider, &DSlider::valueChanged, [=](int value){
            blockInputVolumeSignal = true;
            int sliderInterval = m_SetInputVolumeRecorder.elapsed();
            if (sliderInterval > 50) {
                if (qAbs(value - m_source->volume() * 100) > 1) {
                    m_source->SetVolume(value / 100.0, false);
                }
            }
            m_SetInputVolumeRecorder.restart();

            // send the last change
            if(!m_SetInputVolumeTimer) {
                m_SetInputVolumeTimer = new QTimer(this);
                m_SetInputVolumeTimer->setSingleShot(true);

                connect(m_SetInputVolumeTimer, &QTimer::timeout, m_inputVolumeSlider, [this] {
                    m_SetInputVolumeTimer->deleteLater();
                    m_SetInputVolumeTimer = NULL;
                    int sliderValue = m_inputVolumeSlider->value();
                    if (qAbs(sliderValue - m_source->volume() * 100) > 1) {
                        m_source->SetVolume(sliderValue / 100.0, false);
                    }
                    blockInputVolumeSignal = false;
                });
            }
            m_SetInputVolumeTimer->start(200);
        });
        connect(m_source, &DBusAudioSource::VolumeChanged, [=]{
            if (qAbs(m_source->volume() * 100 - m_inputVolumeSlider->value()) > 1) {
                if (!blockInputVolumeSignal) {
                    m_inputVolumeSlider->setValue(m_source->volume() * 100);
                }
            }
        });

        connect(m_source, &DBusAudioSource::MuteChanged, [=]{
            m_microphoneExpand->setExpand(!m_source->mute());
        });
        connect(m_microphoneExpand, &DBaseExpand::expandChange, [=](bool expanded){
            m_source->SetMute(!expanded);
        });

        connect(m_inputPortsList, &DButtonList::buttonCheckedIndexChanged, [=](int index){
            SourcePortStruct port = m_source->ports().at(index);
            m_source->SetPort(port.id);
        });
        connect(m_source, &DBusAudioSource::ActivePortChanged, [=]{
            QStringList inputPorts;
            foreach (SourcePortStruct port, m_source->ports()) {
                inputPorts << port.name;
            }
            SourcePortStruct activePort = m_source->activePort();
            if (inputPorts.contains(activePort.name)) {
                m_inputPortsList->checkButtonByIndex(inputPorts.indexOf(activePort.name));
            }

            updateMicrophoneUI();
        });

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

    qWarning() << "getDefaultSink is empty";

    return NULL;
}

DBusAudioSource * Sound::getDefaultSource()
{
    foreach (DBusAudioSource * s, m_sources) {
        if (s->name() == m_dbusAudio->defaultSource()) {
            return s;
        }
    }

    qWarning() << "getDefaultSource is empty";

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

    if(!m_source)
        return;

#ifndef DCC_DISABLE_MICROPHONE_FEEDBACK
    // init meter
    QString meterPath = m_source->GetMeter().value().path();
    QString meterName = meterPath;
    meterName = meterName.replace("/", ".").mid(1);

    if(m_dbusMeter)
        m_dbusMeter->deleteLater();

    m_dbusMeter = new QDBusInterface("com.deepin.daemon.Audio", meterPath, meterName);
    m_dbusMeter->setParent(this);
    connect(&m_meterTimer, &QTimer::timeout, m_dbusMeter, [&]{
        m_dbusMeter->call("Tick");
    });

    QDBusConnection::sessionBus().connect(m_dbusMeter->service(), m_dbusMeter->path(), "org.freedesktop.DBus.Properties",  "PropertiesChanged","sa{sv}as", this, SLOT(meterVolumeChanged(QDBusMessage)));

    m_meterTimer.start(2000);
    m_dbusMeter->call("Tick");
#endif
}

void Sound::updateSpeakerUI()
{
    m_outputVolumeSlider->setValue(m_sink->volume() * 100);
    m_leftRightBalanceSlider->setValue(m_sink->balance() * 100);
    m_speakerExpand->setExpand(!m_sink->mute());

}

void Sound::updateMicrophoneUI()
{
    m_inputVolumeSlider->setValue(m_source->volume() * 100);
    m_microphoneExpand->setExpand(!m_source->mute());
}

void Sound::updateOutputPorts()
{
    qDebug() << "updateOutputPorts" << m_sink;
    m_outputPortsList->clear();
    m_outputPortsList->setFixedSize(DCC::ModuleContentWidth, 0);

    if (!m_sink || m_sink->ports().isEmpty())
        return;

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

    }
}

void Sound::updateInputPorts()
{
    qDebug() << "updateInputPorts "<<m_source;

    m_inputPortsList->clear();
    m_inputPortsList->setFixedSize(DCC::ModuleContentWidth, 0);

    if (!m_source|| m_source->ports().isEmpty()) {
        return;
    }

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

    } else {
        m_outputDevicesList->setFixedSize(DCC::ModuleContentWidth, 0);
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

    } else {
        m_inputDevicesList->setFixedSize(DCC::ModuleContentWidth, 0);
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
