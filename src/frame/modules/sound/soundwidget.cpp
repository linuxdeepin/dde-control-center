/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "soundwidget.h"
#include "widgets/dccslider.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QGSettings>

using namespace dcc;

namespace dcc {
namespace sound {

SoundWidget::SoundWidget(SoundModel *model) :
    ModuleWidget(),
    m_speakerGroup(new SettingsGroup),
    m_speakerSwitch(new SwitchWidget),
    m_outputVolumeSliderItem(new TitledSliderItem(tr("Output Volume"))),
    m_outputBalanceSliderItem(new TitledSliderItem(tr("Left/Right Balance"))),
    m_microphoneGroup(new SettingsGroup),
    m_microphoneSwitch(new SwitchWidget),
    m_inputVolumeSliderItem(new TitledSliderItem(tr("Input Volume"))),
#ifndef DCC_DISABLE_FEEDBACK
    m_inputFeedbackSliderItem(new TitledSliderItem(tr("Feedback Volume"))),
#endif
    m_advancedSettingsGroup(new SettingsGroup),
    m_advancedSettingsItem(new NextPageWidget),
    m_soundEffectGroup(new SettingsGroup),
    m_soundEffectBtn(new NextPageWidget)
{
    setObjectName("Sound");

    setTitle(tr("Sound"));

    m_speakerSwitch->setTitle(tr("Speaker"));

    m_gsettings = new QGSettings("com.deepin.dde.audio", "", this);

    m_outputVolumeSliderItem->setObjectName("OutputVolumeSliderItem");
    m_outputVolumeSlider = m_outputVolumeSliderItem->slider();
    m_outputVolumeSlider->setOrientation(Qt::Horizontal);

    // set sound slider max value
    onGSettingsChanged("outputVolumeMax");

    m_outputBalanceSliderItem->setObjectName("OutputBalanceSliderItem");
    m_outputBalanceSlider = m_outputBalanceSliderItem->slider();
    m_outputBalanceSlider->setType(DCCSlider::Vernier);
    m_outputBalanceSlider->setOrientation(Qt::Horizontal);
    m_outputBalanceSlider->setRange(-100, 100);
    m_outputBalanceSlider->setTickInterval(100);
    m_outputBalanceSlider->setTickPosition(QSlider::TicksBelow);

    m_speakerGroup->appendItem(m_speakerSwitch);
    m_speakerGroup->appendItem(m_outputVolumeSliderItem);
    m_speakerGroup->appendItem(m_outputBalanceSliderItem);

    m_microphoneSwitch->setTitle(tr("Microphone"));

    m_inputVolumeSliderItem->setObjectName("InputVolumeSliderItem");
    m_inputVolumeSlider = m_inputVolumeSliderItem->slider();
    m_inputVolumeSlider->setOrientation(Qt::Horizontal);
    m_inputVolumeSlider->setRange(0, 150);

#ifndef DCC_DISABLE_FEEDBACK
    m_inputFeedbackSliderItem->setObjectName("InputFeedbackSliderItem");
    m_inputFeedbackSlider = m_inputFeedbackSliderItem->slider();
    m_inputFeedbackSlider->setType(DCCSlider::Progress);
    m_inputFeedbackSlider->setOrientation(Qt::Horizontal);
    m_inputFeedbackSlider->setRange(0, 100);
    m_inputFeedbackSlider->installEventFilter(this);
#endif

    m_microphoneGroup->appendItem(m_microphoneSwitch);
    m_microphoneGroup->appendItem(m_inputVolumeSliderItem);

#ifndef DCC_DISABLE_FEEDBACK
    m_microphoneGroup->appendItem(m_inputFeedbackSliderItem);
#endif

    m_advancedSettingsItem->setTitle(tr("Advanced"));
    m_advancedSettingsGroup->appendItem(m_advancedSettingsItem);

    m_soundEffectBtn->setTitle(tr("Sound Effects"));
    m_soundEffectGroup->appendItem(m_soundEffectBtn);

    m_centralLayout->addWidget(m_speakerGroup);
    m_centralLayout->addWidget(m_microphoneGroup);
    m_centralLayout->addWidget(m_advancedSettingsGroup);
    m_centralLayout->addWidget(m_soundEffectGroup);

    setModel(model);

    connect(m_speakerSwitch, &SwitchWidget::checkedChanged, this, &SoundWidget::requestSwitchSpeaker);
    connect(m_microphoneSwitch, &SwitchWidget::checkedChanged, this, &SoundWidget::requestSiwtchMicrophone);
    connect(m_soundEffectBtn, &NextPageWidget::clicked, this, &SoundWidget::requestEffectPage);
    connect(m_outputBalanceSlider, &DCCSlider::valueChanged, [this] (double value) { Q_EMIT requestSetSpeakerBalance(value / 100.f); });
    connect(m_inputVolumeSlider, &DCCSlider::valueChanged, [this] (double value) { Q_EMIT requestSetMicrophoneVolume(value / 100.f); });
    connect(m_outputVolumeSlider, &DCCSlider::valueChanged, [this] (double value) { Q_EMIT requestSetSpeakerVolume(value / 100.f);} );
    connect(m_advancedSettingsItem, &NextPageWidget::clicked, this, &SoundWidget::requestAdvancedPage);
    connect(m_gsettings, &QGSettings::changed, this, &SoundWidget::onGSettingsChanged);
}

SoundWidget::~SoundWidget()
{

}

void SoundWidget::setModel(SoundModel *model)
{
    connect(model, &SoundModel::speakerOnChanged, this, [this] (bool on) {
        m_speakerSwitch->setChecked(on);
        m_outputVolumeSliderItem->setVisible(on);
        m_outputBalanceSliderItem->setVisible(on);
    });
    connect(model, &SoundModel::microphoneOnChanged, this, [this] (bool on) {
        m_microphoneSwitch->setChecked(on);
        m_inputVolumeSliderItem->setVisible(on);
#ifndef DCC_DISABLE_FEEDBACK
        m_inputFeedbackSliderItem->setVisible(on);
#endif
    });
    connect(model, &SoundModel::speakerVolumeChanged, this, [this] (double value) {
        m_outputVolumeSlider->blockSignals(true);
        m_outputVolumeSlider->setValue(value * 100);
        m_outputVolumeSlider->blockSignals(false);
    });
    connect(model, &SoundModel::speakerBalanceChanged, this, [this] (double value) {
        m_outputBalanceSlider->blockSignals(true);
        m_outputBalanceSlider->setValue(value * 100);
        m_outputBalanceSlider->blockSignals(false);
    });
    connect(model, &SoundModel::microphoneVolumeChanged, this, [this] (double value) {
        m_inputVolumeSlider->blockSignals(true);
        m_inputVolumeSlider->setValue(value * 100);
        m_inputVolumeSlider->blockSignals(false);
    });

#ifndef DCC_DISABLE_FEEDBACK
    connect(model, &SoundModel::microphoneFeedbackChanged, this, [this] (double value) {
        m_inputFeedbackSlider->setValue(value * 100);
    });
#endif

    blockSignals(true);
    m_speakerSwitch->setChecked(model->speakerOn());
    m_microphoneSwitch->setChecked(model->microphoneOn());
    m_outputVolumeSlider->setValue(model->speakerVolume());
    m_outputBalanceSlider->setValue(model->speakerBalance());
    m_inputVolumeSlider->setValue(model->microphoneVolume());

#ifndef DCC_DISABLE_FEEDBACK
    m_inputFeedbackSlider->setValue(model->microphoneFeedback());
#endif
    blockSignals(false);
}

#ifndef DCC_DISABLE_FEEDBACK
bool SoundWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_inputFeedbackSlider) {
        switch (event->type()) {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            return true;
        default:
            return false;
        }
    }

    return false;
}
#endif

void SoundWidget::onGSettingsChanged(const QString &name)
{
    if (name == "outputVolumeMax") {
        m_outputVolumeSlider->setRange(0, std::min(m_gsettings->get("output-volume-max").toInt(), 150));
    }
}

}
}
