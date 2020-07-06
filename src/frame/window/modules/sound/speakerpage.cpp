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

#include "speakerpage.h"
#include "modules/sound/soundmodel.h"
#include "widgets/switchwidget.h"
#include "window/utils.h"
#include "widgets/titlelabel.h"

#include <DStyle>
#include <DTipLabel>

#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;
DWIDGET_USE_NAMESPACE

const double BrightnessMaxScale = 100.0;
const int PercentageNum = 100;

SpeakerPage::SpeakerPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
{
    m_sw = new SwitchWidget(nullptr, new TitleLabel(tr("Speaker")));
    m_sw->setAccessibleName(tr("Speaker"));

    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->addWidget(m_sw);
    m_layout->addStretch(1);
    setLayout(m_layout);
}

SpeakerPage::~SpeakerPage()
{
}

void SpeakerPage::setModel(dcc::sound::SoundModel *model)
{
    m_model = model;

    m_sw->setChecked(m_model->speakerOn());
    //连接switch点击信号，发送切换开/关扬声器的请求信号
    connect(m_sw, &SwitchWidget::checkedChanged, this, &SpeakerPage::requestSwitchSpeaker);
    //当扬声器状态发生变化，将switch设置为对应的状态
    connect(m_model, &SoundModel::speakerOnChanged, m_sw, &SwitchWidget::setChecked);

    initSlider();
}

void SpeakerPage::initSlider()
{
    //~ contents_path /sound/Speaker
    m_outputSlider = new TitledSliderItem(tr("Output Volume"), this);
    m_outputSlider->addBackground();
    m_outputSlider->setVisible(m_model->speakerOn());
    m_speakSlider = m_outputSlider->slider();

    QStringList annotions;
    if (m_model->MaxUIVolume() > 1.0) {
        annotions << "0" << " " << "100" << "150";
        m_speakSlider->setAnnotations(annotions);
    }else {
        annotions << "0" << " " << "100";
        m_speakSlider->setAnnotations(annotions);
    }

    //初始化音量设置滚动条
    qDebug() << "max volume:" << m_model->MaxUIVolume();
    int maxRange = static_cast<int>(m_model->MaxUIVolume() * BrightnessMaxScale);
    m_speakSlider->setRange(0, maxRange);
    m_speakSlider->setType(DCCSlider::Vernier);
    m_speakSlider->setTickPosition(QSlider::NoTicks);
    //从DStyle 中获取标准图标
    auto icon_low = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeLowElement);
    m_outputSlider->setLeftIcon(icon_low);
    //从DStyle 中获取标准图标
    auto icon_high = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeHighElement);
    m_outputSlider->setRightIcon(icon_high);
    m_outputSlider->setIconSize(QSize(24, 24));

    m_speakSlider->setTickInterval(1);
    qDebug() << "speaker volume:" << m_model->speakerVolume();
    int val = qRound(m_model->speakerVolume() * BrightnessMaxScale);
    if (val > maxRange) {
        val = maxRange;
    }
    m_speakSlider->setValue(val);
    m_outputSlider->setValueLiteral(QString::number(val) + "%");
    m_speakSlider->setPageStep(1);

    //处理滑块位置变化的槽
    auto slotfunc1 = [ = ](int pos) {
        double val = pos / BrightnessMaxScale;
        //滑块位置改变时，发送设置音量的信号
        Q_EMIT requestSetSpeakerVolume(val);
    };
    //当点击滑槽时不会有，sliderMoved消息，用这个补
    connect(m_speakSlider, &DCCSlider::valueChanged, slotfunc1);
    //滑块移动消息处理
    connect(m_speakSlider, &DCCSlider::sliderMoved, slotfunc1);
    //当扬声器开/关时，显示/隐藏控件
    connect(m_model, &SoundModel::speakerOnChanged, m_outputSlider, &TitledSliderItem::setVisible);
    //当底层数据改变后，更新滑动条显示的数据
    connect(m_model, &SoundModel::speakerVolumeChanged, this, [ = ](double v) {
        m_speakSlider->blockSignals(true);
        m_speakSlider->setValue(v * 100);
        m_speakSlider->blockSignals(false);
        m_outputSlider->setValueLiteral(QString::number(v * 100) + "%");
    });

    connect(m_model, &SoundModel::maxUIVolumeChanged, this, [ = ](double maxvalue) {
        m_speakSlider->setRange(0, qRound(maxvalue * PercentageNum));
        QStringList annotions;
        if (maxvalue > 1.0) {
            annotions << "0 " << "" << "100" << "150 ";
            qDebug() << m_outputSlider << annotions;
            m_outputSlider->slider()->setRightTicks(annotions);
        }else{
            annotions << "0 " << "" << "100";
            m_outputSlider->slider()->setRightTicks(annotions);
        }
        m_outputSlider->update();

        m_speakSlider->blockSignals(true);
        m_speakSlider->setValue(qRound(m_model->speakerVolume() * PercentageNum));
        m_speakSlider->blockSignals(false);
        m_outputSlider->setValueLiteral(QString::number(qRound(m_model->speakerVolume() * PercentageNum)) + "%");
    });

    m_layout->insertWidget(1, m_outputSlider);

    //音量增强
    auto hlayout = new QVBoxLayout();
    auto volumeBoost = new SwitchWidget(this);
    volumeBoost->setChecked(m_model->isIncreaseVolume());
    volumeBoost->setTitle(tr("Volume Boost"));
    volumeBoost->addBackground();
    connect(m_model, &SoundModel::increaseVolumeChanged, volumeBoost, &SwitchWidget::setChecked);
    connect(volumeBoost, &SwitchWidget::checkedChanged, this, &SpeakerPage::requestIncreaseVolume);
    hlayout->addSpacing(4);
    hlayout->addWidget(volumeBoost);
    volumeBoost->setVisible(false);//去掉音量增强
    hlayout->addSpacing(4);
    hlayout->setContentsMargins(0, 0, 0, 0);

    //下方提示
    auto volumeBoostTip = new DTipLabel(tr("If the volume is louder than 100%, it may distort audio and be harmful to your speaker"), this);
    volumeBoostTip->setWordWrap(true);
    volumeBoostTip->setMargin(0);
    volumeBoostTip->setVisible(m_model->isIncreaseVolume());
    hlayout->addWidget(volumeBoostTip);
    auto vbWidget = new QWidget(this);
    vbWidget->setLayout(hlayout);
    vbWidget->setVisible(m_model->speakerOn());
    m_layout->insertWidget(2, vbWidget);
    connect(volumeBoost, &SwitchWidget::checkedChanged, volumeBoostTip, &DTipLabel::setVisible);
    connect(m_model, &SoundModel::increaseVolumeChanged, volumeBoostTip, &DTipLabel::setVisible);
    connect(m_model, &SoundModel::speakerOnChanged, vbWidget, &QWidget::setVisible);
    //~ contents_path /sound/Speaker
    auto balanceSlider = new TitledSliderItem(tr("Left/Right Balance"), this);
    balanceSlider->addBackground();
    balanceSlider->setVisible(m_model->speakerOn());

    //信号处理与上面一致
    QStringList balanceList;
    balanceList << tr("Left")<< " ";
    balanceList << tr("Right");
    DCCSlider *slider2 = balanceSlider->slider();
    slider2->setRange(-100, 100);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::TicksBelow);
    slider2->setTickInterval(1);
    slider2->setSliderPosition(qRound(m_model->speakerBalance() * PercentageNum));
    slider2->setPageStep(1);
    slider2->setEnabledAcrossStyle(true);
    balanceSlider->setAnnotations(balanceList);

    auto slotfunc2 = [ = ](int pos) {
        double val = pos / BrightnessMaxScale;
        Q_EMIT requestSetSpeakerBalance(val);
    };
    connect(slider2, &DCCSlider::valueChanged, slotfunc2);
    connect(slider2, &DCCSlider::sliderMoved, slotfunc2);
    connect(m_model, &SoundModel::speakerOnChanged, balanceSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::speakerBalanceChanged, this, [ = ](double v) {
        slider2->blockSignals(true);
        slider2->setSliderPosition(qRound(v * PercentageNum));
        slider2->blockSignals(false);
    });

    if (!volumeBoost->isVisible())
        m_layout->insertWidget(2, balanceSlider);
    else
        m_layout->insertWidget(3, balanceSlider);
}
