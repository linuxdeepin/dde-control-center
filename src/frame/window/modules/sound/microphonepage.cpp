/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "microphonepage.h"
#include "modules/sound/soundmodel.h"
#include "window/utils.h"

#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/comboxwidget.h"

#include <DStandardItem>
#include <DStyle>
#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;

Q_DECLARE_METATYPE(const dcc::sound::Port *)

MicrophonePage::MicrophonePage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_sw(new SwitchWidget)
{
    const int titleLeftMargin = 17;
    //~ contents_path /sound/Advanced
    TitleLabel *labelInput = new TitleLabel(tr("Input"));
    DFontSizeManager::instance()->bind(labelInput, DFontSizeManager::T5, QFont::DemiBold);
    labelInput->setContentsMargins(titleLeftMargin, 0, 0, 0);
    labelInput->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_inputSoundCbx = new ComboxWidget(tr("Input Device"));

    QHBoxLayout *hlayout = new QHBoxLayout;
    TitleLabel *lblTitle = new TitleLabel(tr("Microphone"));
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_sw = new SwitchWidget(nullptr, lblTitle);

    TitleLabel *ndTitle = new TitleLabel(tr("Automatic Noise Suppression"));
    DFontSizeManager::instance()->bind(ndTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_noiseReductionsw = new SwitchWidget(nullptr, ndTitle);

    hlayout->addWidget(m_sw);

    m_inputModel  = new QStandardItemModel(m_inputSoundCbx->comboBox());
    m_inputSoundCbx->comboBox()->setModel(m_inputModel);

    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->addWidget(labelInput);
    m_layout->addWidget(m_inputSoundCbx);
    m_layout->addLayout(hlayout);
    m_layout->addWidget(m_noiseReductionsw);
    setLayout(m_layout);
}


MicrophonePage::~MicrophonePage()
{
#ifndef DCC_DISABLE_FEEDBACK
    if (m_feedbackSlider)
        m_feedbackSlider->disconnect(m_conn);
    m_feedbackSlider->deleteLater();
#endif
}

void MicrophonePage::setModel(SoundModel *model)
{
    m_model = model;

    //监听消息设置是否可用
    connect(m_model, &SoundModel::isPortEnableChanged, this, [ = ](bool enable) {
        m_sw->setChecked(enable);
    });
    //发送查询请求消息看是否可用
    connect(m_model, &SoundModel::setPortChanged, this, [ = ](const dcc::sound::Port  * port) {
        m_currentPort = port;
        Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->cardName());
    });

    auto ports = m_model->ports();
    for (auto port : ports) {
        addPort(port);
    }
    //这个临时这个判断,直接设置m_noiseReductionsw(m_model->reduceNoise()) 没有效果,待dtk组的同时祥查
    if (m_model->reduceNoise())
        m_noiseReductionsw->setChecked(true);
    else
        m_noiseReductionsw->setChecked(false);

    //连接switch点击信号，发送切换开/关扬声器的请求信号
    connect(m_sw, &SwitchWidget::checkedChanged, this, [ = ] {
        if(m_currentPort != nullptr)
            Q_EMIT m_model->requestSwitchSetEnable(m_currentPort->cardId(), m_currentPort->cardName(), m_sw->checked());
        else
            m_sw->setChecked(false);
    });

    connect(m_model, &SoundModel::portAdded, this, &MicrophonePage::addPort);

    connect(m_model, &SoundModel::portRemoved, this, &MicrophonePage::removePort);

    connect(m_inputSoundCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, [this](const int  idx) {
        auto temp = m_inputModel->index(idx, 0);
        this->requestSetPort(m_inputModel->data(temp, Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>());
    });

    connect(m_noiseReductionsw, &SwitchWidget::checkedChanged, this, &MicrophonePage::requestReduceNoise);

    initSlider();
}

void MicrophonePage::removePort(const QString &portId, const uint &cardId)
{
    auto rmFunc = [ = ](QStandardItemModel * model) {
        for (int i = 0; i < model->rowCount();) {
            auto item = model->item(i);
            auto port = item->data(Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>();
            if (port->id() == portId && cardId == port->cardId()) {
                model->removeRow(i);
            } else {
                ++i;
            }
        }
    };

    rmFunc(m_inputModel);
}

void MicrophonePage::addPort(const dcc::sound::Port *port)
{
    if (port->In == port->direction()) {
        DStandardItem *pi = new DStandardItem;
        pi->setText(port->name());

        pi->setData(QVariant::fromValue<const dcc::sound::Port *>(port), Qt::WhatsThisPropertyRole);

        connect(port, &dcc::sound::Port::nameChanged, this, [ = ](const QString str) {
            pi->setText(str);
        });
        connect(port, &dcc::sound::Port::isActiveChanged, this, [ = ](bool isActive) {
            pi->setCheckState(isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        });

        m_inputModel->appendRow(pi);
        if (port->isActive()) {
            m_inputSoundCbx->comboBox()->setCurrentText(port->name());
            m_currentPort = port;
            Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->cardName());
        }
    }
}

void MicrophonePage::initSlider()
{
    //~ contents_path /sound/Microphone
    m_inputSlider = new TitledSliderItem(tr("Input Volume"));
    m_inputSlider->addBackground();
    m_inputSlider->setVisible(m_model->isPortEnable());
    m_layout->insertWidget(3, m_inputSlider);


    DCCSlider *slider = m_inputSlider->slider();
    slider->setRange(0, 100);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::NoTicks);
    auto icon_low = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeLowElement);
    slider->setLeftIcon(icon_low);
    auto icon_high = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeHighElement);
    slider->setRightIcon(icon_high);
    slider->setIconSize(QSize(24, 24));
    slider->setTickInterval(1);
    slider->setSliderPosition(int(m_model->microphoneVolume() * 100));
    slider->setPageStep(1);

    auto slotfunc1 = [ = ](int pos) {
        double val = pos / 100.0;
        Q_EMIT requestSetMicrophoneVolume(val);
    };
    int val = static_cast<int>(m_model->microphoneVolume() * static_cast<int>(100.0f));
    slider->setValue(val);
    m_inputSlider->setValueLiteral(QString::number(m_model->microphoneVolume() * 100) + "%");
    connect(slider, &DCCSlider::valueChanged, this, slotfunc1);
    connect(slider, &DCCSlider::sliderMoved, this, slotfunc1);
    connect(m_model, &SoundModel::isPortEnableChanged, m_inputSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::microphoneVolumeChanged, this, [ = ](double v) {
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(v * 100));
        slider->setSliderPosition(static_cast<int>(v * 100));
        slider->blockSignals(false);
        m_inputSlider->setValueLiteral(QString::number(v * 100) + "%");
    });
#ifndef DCC_DISABLE_FEEDBACK
    //~ contents_path /sound/Microphone
    m_feedbackSlider = (new TitledSliderItem(tr("Input Level")));
    m_feedbackSlider->addBackground();
    m_feedbackSlider->setVisible(m_model->isPortEnable());
    m_noiseReductionsw->setVisible(m_model->isPortEnable());
    DCCSlider *slider2 = m_feedbackSlider->slider();
    slider2->setRange(0, 100);
    slider2->setEnabled(false);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::NoTicks);
    slider2->setLeftIcon(QIcon::fromTheme("dcc_feedbacklow"));
    slider2->setRightIcon(QIcon::fromTheme("dcc_feedbackhigh"));
    slider2->setIconSize(QSize(24, 24));
    slider2->setTickInterval(1);
    slider2->setPageStep(1);

    connect(m_model, &SoundModel::isPortEnableChanged, m_feedbackSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::isPortEnableChanged, m_noiseReductionsw, &ComboxWidget::setVisible);
    m_conn = connect(m_model, &SoundModel::microphoneFeedbackChanged, [ = ](double vol2) {
        slider2->setSliderPosition(int(vol2 * 100));
    });
    m_layout->setSpacing(10);
    m_layout->insertWidget(4, m_feedbackSlider);
    m_layout->addStretch(10);
#endif
}
