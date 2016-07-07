#include "soundview.h"
#include "soundcontrol.h"

#include <QApplication>
#include <QScreen>

#include <dswitchlineexpand.h>
#include <dslider.h>
#include <dbuttonlist.h>
#include <dbaseexpand.h>
#include <dscrollarea.h>
#include <darrowlineexpand.h>
#include <dboxwidget.h>
#include <dlinkbutton.h>

#include "constants.h"
#include "moduleheader.h"
#include "headerline.h"
#include "normallabel.h"
#include "soundeffectswitchwidget.h"

namespace Plugin
{
namespace Sound
{

static const int TitleLabelWidth = 110;

SoundView::SoundView(SoundControl *control, QWidget *parent):
    QFrame(parent),
    m_control(control)
{
    qDebug() << "SoundView init";

    // add
    connect(this, &SoundView::requestInit, m_control, &SoundControl::init);
    connect(m_control, &SoundControl::initialized, this, &SoundView::init);

    emit this->requestInit();
}

void SoundView::initAdvanceOptions(const SoundModel &model)
{
    // advanced settings expand widget
    DVBoxWidget *advanced_widget = new DVBoxWidget;
    advanced_widget->setFixedWidth(DCC::ModuleContentWidth);

    QVBoxLayout *advanced_layout = new QVBoxLayout;
    advanced_layout->setMargin(0);
    advanced_layout->setSpacing(0);
    advanced_widget->layout()->addLayout(advanced_layout);

    DBaseExpand *advanced_expand = new DBaseExpand;
    advanced_expand->setContent(advanced_widget);
    advanced_expand->setSeparatorVisible(false);
    advanced_expand->setExpandedSeparatorVisible(false);

    ///////////////////////////////////////////////////////--Advanced settings
    // Output ports
    m_outputPortsExpand = new DBaseExpand;
    m_outputPortsExpand->setExpand(true);

    HeaderLine *outputPortsLine = new HeaderLine(tr("Output Port"), m_outputPortsExpand);
    m_outputPortsExpand->setHeader(outputPortsLine);

    m_outputPortsList = new DButtonList(m_outputPortsExpand);
    m_outputPortsExpand->setContent(m_outputPortsList);
    updateOutputPortsUI(model);
    advanced_layout->addWidget(m_outputPortsExpand);

    // Input ports
    m_inputPortsExpand = new DBaseExpand;
    m_inputPortsExpand->setExpand(true);

    HeaderLine *inputPortsLine = new HeaderLine(tr("Input Port"), m_inputPortsExpand);
    m_inputPortsExpand->setHeader(inputPortsLine);

    m_inputPortsList = new DButtonList(m_inputPortsExpand);
    m_inputPortsExpand->setContent(m_inputPortsList);

    updateInputPortsUI(model);

    advanced_layout->addWidget(m_inputPortsExpand);
    advanced_layout->addWidget(new DSeparatorHorizontal);
    advanced_layout->addStretch(1);

    // show advanced
    DLinkButton *advanced_button = new DLinkButton(tr("Show Advanced...") + "      ");
    advanced_button->setStyleSheet(advanced_button->styleSheet() + "Dtk--Widget--DLinkButton:pressed{color:#0188FF;}");

    connect(advanced_button, &DLinkButton::clicked, [this, advanced_button, advanced_expand] {
        if (advanced_expand->expand())
        {
            //            this->setFixedHeight(150);
            ///When the height of the sum of the hide when advanced settings
            advanced_expand->setExpand(false);
            advanced_button->setText(tr("Show Advanced...") + "      ");
        } else{
            //            this->setFixedHeight(650);
            ///When the height of the sum of the show when advanced settings
            advanced_expand->setExpand(true);
            advanced_button->setText(tr("Hide Advanced...") + "      ");
        }
    });
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(advanced_expand);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(advanced_button, 0, Qt::AlignRight);

    connect(m_control, &SoundControl::cardsChanged, this, &SoundView::updateOutputPortsUI);

    connect(m_outputPortsList, &DButtonList::buttonCheckedIndexChanged,
    [&](int index) {
        emit portChange(m_sinkPorts[index].card, m_sinkPorts[index].name, Output);
    });

    connect(m_control, &SoundControl::outputActivePortChanged, [ = ](const SoundModel & model) {
        for (int index = 0; index < m_sinkPorts.length(); ++index) {
            if (m_sinkPorts.at(index).name == model.activeSinkPort) {
                m_outputPortsList->checkButtonByIndex(index);
            }
        }
        updateSpeakerUI(model);
    });

    connect(m_control, &SoundControl::cardsChanged, this, &SoundView::updateInputPortsUI);
    connect(m_inputPortsList, &DButtonList::buttonCheckedIndexChanged,
    [&](int index) {
        emit portChange(m_sourcePorts[index].card, m_sourcePorts[index].name, Input);
    });

    connect(this, &SoundView::portChange, m_control, &SoundControl::setPort);

    connect(m_control, &SoundControl::inputActivePortChanged, [ = ](const SoundModel & model) {
        for (int index = 0; index < m_sourcePorts.length(); ++index) {
            if (m_sourcePorts.at(index).name == model.activeSourcePort) {
                m_inputPortsList->checkButtonByIndex(index);
            }
        }
        updateMicrophoneUI(model);
    });
}

void SoundView::initEffectOptions(const SoundModel &model)
{
    DSwitchButton *soundEffectsSwitch = new DSwitchButton;
    m_soundEffectsLine = new DHeaderLine;
    m_soundEffectsLine->setTitle(tr("Sound effects"));
    m_soundEffectsLine->setContent(soundEffectsSwitch);
    soundEffectsSwitch->setChecked(model.effect.enabled);
    m_mainLayout->addWidget(m_soundEffectsLine);

    connect(soundEffectsSwitch, &DSwitchButton::checkedChanged,
            m_control, &SoundControl::setEffectsEnabled);

    connect(m_control, &SoundControl::effectsEnabledChanged,
            soundEffectsSwitch, &DSwitchButton::setChecked);
}

void SoundView::initOutputOption(const SoundModel &model)
{
    m_speakerExpand = new DSwitchLineExpand;
    m_speakerExpand->setTitle(tr("Speaker"));

    QFrame *speakerExpandContent = new QFrame(m_speakerExpand);
    speakerExpandContent->setFixedWidth(DCC::ModuleContentWidth);
    speakerExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QGridLayout *speakerForm = new QGridLayout(speakerExpandContent);
    speakerForm->setRowMinimumHeight(0, 36);
    speakerForm->setRowMinimumHeight(1, 36);

    // Output volume line
    NormalLabel *outputVolumeLabel = new NormalLabel(tr("Output Volume"));
    outputVolumeLabel->setFixedWidth(TitleLabelWidth);
    outputVolumeLabel->setWordWrap(true);

    speakerForm->addWidget(outputVolumeLabel, 0, 0, Qt::AlignVCenter);
    m_outputVolumeSlider = new DSlider(Qt::Horizontal);
    m_outputVolumeSlider->setRange(0, 150);
    m_outputVolumeSlider->setLeftTip("-");
    m_outputVolumeSlider->setRightTip("+");
    m_outputVolumeSlider->addScale(100);
    m_outputVolumeSlider->setHoverShowValue(true);
    m_outputVolumeSlider->setHoverShowValueInterval(1000);
    speakerForm->addWidget(m_outputVolumeSlider, 0, 1, Qt::AlignVCenter);

    // Left/Right balance line
    NormalLabel *balanceLabel = new NormalLabel(tr("Left/Right Balance"));
    balanceLabel->setFixedWidth(TitleLabelWidth);
    balanceLabel->setWordWrap(true);

    speakerForm->addWidget(balanceLabel, 1, 0, Qt::AlignVCenter);
    m_leftRightBalanceSlider = new DSlider(Qt::Horizontal);
    m_leftRightBalanceSlider->setHandleType(DSlider::SharpHandler);
    m_leftRightBalanceSlider->setRange(-100, 100);
    m_leftRightBalanceSlider->setLeftTip(tr("Left"));
    m_leftRightBalanceSlider->setRightTip(tr("Right"));
    m_leftRightBalanceSlider->addScale(0);
    m_leftRightBalanceSlider->setHoverShowValue(true);
    m_leftRightBalanceSlider->setHoverShowValueInterval(1000);
    speakerForm->addWidget(m_leftRightBalanceSlider, 1, 1, Qt::AlignVCenter);

    updateSpeakerUI(model);
    speakerExpandContent->adjustSize();
    speakerExpandContent->setFixedHeight(speakerExpandContent->height());

    m_speakerExpand->setContent(speakerExpandContent);
    m_mainLayout->addWidget(m_speakerExpand);

    connect(m_control, &SoundControl::outputChanged, this, &SoundView::updateSpeakerUI);

    connect(m_speakerExpand, &DBaseExpand::expandChange, [ = ](bool expanded) {
        m_control->setMute(!expanded);
    });

    connect(m_control, &SoundControl::outputMuteChanged, this, [ = ](bool mute) {
        m_speakerExpand->setExpand(!mute);
    });

    connect(m_outputVolumeSlider, &DSlider::valueChanged, [ = ](int value) {
        blockOutputVolumeSignal = true;
        int sliderInterval = m_SetOutputVolumeRecorder.elapsed();
        if (sliderInterval > 50) {
            m_control->setOutputVolume(double(value) / 100, false);
        }
        m_SetOutputVolumeRecorder.restart();

        // send the last change
        if (!m_SetOutputVolumeTimer) {
            m_SetOutputVolumeTimer = new QTimer(this);
            m_SetOutputVolumeTimer->setSingleShot(true);

            connect(m_SetOutputVolumeTimer, &QTimer::timeout, m_outputVolumeSlider, [this] {
                m_SetOutputVolumeTimer->deleteLater();
                m_SetOutputVolumeTimer = NULL;
                auto value = m_outputVolumeSlider->value();
                m_control->setOutputVolume(double(value) / 100, false);
                blockOutputVolumeSignal = false;
            });
        }
        m_SetOutputVolumeTimer->start(200);
    });

    connect(m_control, &SoundControl::outputVolumeChanged, this, [ = ](double v) {

        const int value = std::rint(v * 100);

        if (qAbs(value - m_outputVolumeSlider->value()) > 1) {
            if (!blockOutputVolumeSignal) {
                m_outputVolumeSlider->setValue(value);
            }
        }
    });

    connect(m_leftRightBalanceSlider, &DSlider::valueChanged, [ = ](int value) {
        blockBalanceSignal = true;
        int sliderInterval = m_SetBalanceRecorder.elapsed();
        if (sliderInterval > 50) {
            m_control->setBalance(double(value) / 100, false);

        }
        m_SetBalanceRecorder.restart();

        // send the last change
        if (!m_SetBalanceTimer) {
            m_SetBalanceTimer = new QTimer(this);
            m_SetBalanceTimer->setSingleShot(true);

            connect(m_SetBalanceTimer, &QTimer::timeout, m_leftRightBalanceSlider, [this] {
                m_SetBalanceTimer->deleteLater();
                m_SetBalanceTimer = NULL;
                int sliderValue = m_leftRightBalanceSlider->value();
                m_control->setBalance(double(sliderValue) / 100, false);
                blockBalanceSignal = false;
            });
        }
        m_SetBalanceTimer->start(200);
    });

    connect(m_control, &SoundControl::outputBalanceChanged, this, [ = ](double v) {
        const int value = std::rint(v * 100);

        if (qAbs(value - m_leftRightBalanceSlider->value()) > 1) {
            if (!blockBalanceSignal) {
                m_leftRightBalanceSlider->setValue(value);
            }
        }
    });


    m_speakerExpand->setVisible(model.sink.defaultPortActive);
}

void SoundView::initInputOption(const SoundModel &model)
{
    ///////////////////////////////////////////////////////-- Microphone Settings
    m_microphoneExpand = new DSwitchLineExpand;
    m_microphoneExpand->setTitle(tr("Microphone"));

    QFrame *mircophoneExpandContent = new QFrame(m_microphoneExpand);
    mircophoneExpandContent->setFixedWidth(DCC::ModuleContentWidth);
    mircophoneExpandContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QGridLayout *microphoneForm = new QGridLayout(mircophoneExpandContent);
    microphoneForm->setRowMinimumHeight(0, 36);
#ifndef DCC_DISABLE_MICROPHONE_FEEDBACK
    microphoneForm->setRowMinimumHeight(1, 36);
#endif
    // microphone volume line
    NormalLabel *inputVolumeLabel = new NormalLabel(tr("Input Volume"));
    inputVolumeLabel->setFixedWidth(TitleLabelWidth);
    inputVolumeLabel->setWordWrap(true);

    microphoneForm->addWidget(inputVolumeLabel, 0, 0, Qt::AlignVCenter);
    m_inputVolumeSlider = new DSlider(Qt::Horizontal);
    m_inputVolumeSlider->setRange(0, 150);
    m_inputVolumeSlider->setLeftTip("-");
    m_inputVolumeSlider->setRightTip("+");
    m_inputVolumeSlider->addScale(100);
    m_inputVolumeSlider->setHoverShowValue(true);
    m_inputVolumeSlider->setHoverShowValueInterval(1000);
    microphoneForm->addWidget(m_inputVolumeSlider, 0, 1, Qt::AlignVCenter);

#ifndef DCC_DISABLE_MICROPHONE_FEEDBACK
    // microphone feedback line
    NormalLabel *feedbackVolumeLabel = new NormalLabel(tr("Feedback Volume"));
    feedbackVolumeLabel->setFixedWidth(TitleLabelWidth);
    feedbackVolumeLabel->setWordWrap(true);

    microphoneForm->addWidget(feedbackVolumeLabel, 1, 0, Qt::AlignVCenter);
    m_inputFeedbackSlider = new DSlider(Qt::Horizontal);
    QString feedbackSliderStyle = m_inputFeedbackSlider->styleSheet();
    feedbackSliderStyle += "Dtk--Widget--DSlider::handle[handleType=\"1\"] {background: none;}Dtk--Widget--DSlider::add-page:horizontal[handleType=\"1\"]{border-width: 0px 2px 1px 0px;}";
    m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle);
    m_inputFeedbackSlider->setRange(0, 100);
    m_inputFeedbackSlider->setEnabled(false);

    connect(m_control, &SoundControl::inputMeterVolumeChanged,
            m_inputFeedbackSlider, &DSlider::setValue);

    connect(m_inputFeedbackSlider, &DSlider::valueChanged, [ = ](int value) {
        // to prevent the slider being over refreshed.
        if (m_timeDeltaRecorder.elapsed() < 500) { return; }
        m_timeDeltaRecorder.restart();

        if (value > 80) {
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
                                                 + "Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0a73bb, stop:0.66 #0a73bb, stop:0.76 #ffbf0f, stop:1 #ff8503);}Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}");
        } else if (value < 3) {
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
                                                 + "Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 2px;}");
        } else {
            m_inputFeedbackSlider->setStyleSheet(feedbackSliderStyle
                                                 + "Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0a73bb, stop:1 #51a7dc)}Dtk--Widget--DSlider::sub-page[handleType=\"1\"]{border-image: none;border-radius: 3px;}");
        }
    });
    microphoneForm->addWidget(m_inputFeedbackSlider, 1, 1, Qt::AlignVCenter);
#endif // DCC_DISABLE_MICROPHONE_FEEDBACK

    updateMicrophoneUI(model);
    mircophoneExpandContent->adjustSize();

    m_microphoneExpand->setContent(mircophoneExpandContent);
    m_mainLayout->addWidget(m_microphoneExpand);
    connect(m_control, &SoundControl::inputChanged, this, &SoundView::updateMicrophoneUI);

    connect(m_inputVolumeSlider, &DSlider::valueChanged, [ = ](int value) {
        blockInputVolumeSignal = true;
        int sliderInterval = m_SetInputVolumeRecorder.elapsed();
        if (sliderInterval > 50) {
            m_control->setInputVolume(double(value) / 100, false);

        }
        m_SetInputVolumeRecorder.restart();

        // send the last change
        if (!m_SetInputVolumeTimer) {
            m_SetInputVolumeTimer = new QTimer(this);
            m_SetInputVolumeTimer->setSingleShot(true);

            connect(m_SetInputVolumeTimer, &QTimer::timeout, m_inputVolumeSlider, [this] {
                m_SetInputVolumeTimer->deleteLater();
                m_SetInputVolumeTimer = NULL;
                int sliderValue = m_inputVolumeSlider->value();
                m_control->setInputVolume(double(sliderValue) / 100, false);
                blockInputVolumeSignal = false;
            });
        }
        m_SetInputVolumeTimer->start(200);
    });

    connect(m_control, &SoundControl::inputVolumeChanged, this, [ = ](double v) {

        const int value = std::rint(v * 100);

        if (qAbs(value - m_inputVolumeSlider->value()) > 1) {
            if (!blockInputVolumeSignal) {
                m_inputVolumeSlider->setValue(value);
            }
        }
    });

    connect(m_control, &SoundControl::inputMuteChanged, this, [ = ](bool mute) {
        m_microphoneExpand->setExpand(!mute);
    });
    connect(m_microphoneExpand, &DBaseExpand::expandChange, this, [ = ](bool expanded) {
        m_control->setInputMute(!expanded);
    });
    m_microphoneExpand->setVisible(model.source.defaultSourceActive);
}

void SoundView::init(const SoundModel &model)
{
    m_mainWidgetVLayout = new QVBoxLayout(this);
    m_mainWidgetVLayout->setSpacing(0);
    m_mainWidgetVLayout->setMargin(0);

    m_frame = new QFrame;
    m_frame->setFixedWidth(DCC::ModuleContentWidth);
    m_frame->setObjectName("SoundMainFrame");
    m_frame->setStyleSheet("QFrame#SoundMainFrame{background:transparent}");

    m_scrollArea = new DScrollArea;
    m_scrollArea->setFixedSize(DCC::ModuleContentWidth, qApp->primaryScreen()->geometry().height() - 60);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_frame);

    ///////////////////////////////////////////////////////-- Title
    m_moduleHeader = new ModuleHeader(tr("Sound"), true, this);
    connect(m_moduleHeader, &ModuleHeader::resetButtonClicked, m_control, &SoundControl::reset);
    m_mainWidgetVLayout->addWidget(m_moduleHeader);
    m_mainWidgetVLayout->addWidget(new DSeparatorHorizontal);
    m_mainWidgetVLayout->addWidget(m_scrollArea);
    m_mainWidgetVLayout->addStretch(1);

    m_mainLayout = new QVBoxLayout(m_frame);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    initOutputOption(model);
    initInputOption(model);
    initEffectOptions(model);
    initAdvanceOptions(model);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addStretch(1);

}

void SoundView::updateOutputPortsUI(const SoundModel &model)
{
    m_outputPortsList->clear();
    m_sinkPortsName.clear();
    m_outputPortsList->setFixedSize(DCC::ModuleContentWidth, 0);

    const QString activePort = model.activeSinkPort;
    m_sinkPorts = model.sinkPorts;
    QStringList outputPorts;
    foreach(SoundPort port, model.sinkPorts) {
        m_sinkPortsName << port.name;
        outputPorts << port.description;
    }

    qDebug() << "activePort" << activePort;
    if (outputPorts.length() > 0) {
        m_outputPortsList->addButtons(outputPorts);
        m_outputPortsList->setFixedSize(DCC::ModuleContentWidth, outputPorts.length() * m_outputPortsList->itemWidget(m_outputPortsList->item(0))->height());
        if (m_sinkPortsName.contains(activePort)) {
            m_outputPortsList->checkButtonByIndex(m_sinkPortsName.indexOf(activePort));
        }
        m_outputPortsExpand->show();
    } else {
        m_outputPortsExpand->hide();
    }
}

void SoundView::updateInputPortsUI(const SoundModel &model)
{
    m_inputPortsList->clear();
    m_sourcePortsName.clear();

    QString defaultPort = model.activeSourcePort;
    m_sourcePorts = model.sourcePorts;
    QStringList inputPorts;
    foreach(SoundPort port, model.sourcePorts) {
        m_sourcePortsName << port.name;
        inputPorts << port.description;
    }

    if (inputPorts.length() > 0) {
        m_inputPortsList->addButtons(inputPorts);
        m_inputPortsList->setFixedSize(DCC::ModuleContentWidth,
                                       inputPorts.length() * m_inputPortsList->itemWidget(m_inputPortsList->item(0))->height());
        if (m_sourcePortsName.contains(defaultPort)) {
            m_inputPortsList->blockSignals(true);
            m_inputPortsList->checkButtonByIndex(m_sourcePortsName.indexOf(defaultPort));
            m_inputPortsList->blockSignals(false);
        }
        m_inputPortsExpand->show();
    } else {
        m_inputPortsExpand->hide();
    }
}

void SoundView::updateMicrophoneUI(const SoundModel &model)
{
    m_inputVolumeSlider->setValue(std::rint(model.source.volume * 100));
    m_microphoneExpand->setExpand(!model.source.mute);
    m_microphoneExpand->setVisible(model.source.defaultSourceActive);
}

void SoundView::updateSpeakerUI(const SoundModel &model)
{
    m_outputVolumeSlider->setValue(std::rint(model.sink.volume * 100));
    m_leftRightBalanceSlider->setValue(std::rint(model.sink.balance * 100));
    m_speakerExpand->setExpand(!model.sink.mute);
    m_speakerExpand->setVisible(model.sink.defaultPortActive);
}

SoundView::~SoundView()
{
    qDebug() << "SoundView destory";
}

}
}
