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
#include <DFontSizeManager>
#include <DStandardItem>

#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDebug>

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;

Q_DECLARE_METATYPE(const dcc::sound::Port *)

DWIDGET_USE_NAMESPACE

SpeakerPage::SpeakerPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_outputSlider(nullptr)
    , m_speakSlider(nullptr)
{
    const int titleLeftMargin = 17;
    //~ contents_path /sound/Advanced
    TitleLabel *labelOutput = new TitleLabel(tr("Output"));
    DFontSizeManager::instance()->bind(labelOutput, DFontSizeManager::T5, QFont::DemiBold);
    labelOutput->setContentsMargins(titleLeftMargin, 0, 0, 0);
    labelOutput->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_outputSoundCbx = new ComboxWidget(tr("Output Device"));
    m_outputSoundCbx->setMaximumHeight(100);

    m_outputModel  = new QStandardItemModel(m_outputSoundCbx->comboBox());
    m_outputSoundCbx->comboBox()->setModel(m_outputModel);

    TitleLabel *lblTitle = new TitleLabel(tr("Speaker"));
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_sw = new SwitchWidget(nullptr, lblTitle);
    m_sw->setAccessibleName(tr("Speaker"));

    m_layout->addWidget(labelOutput);
    m_layout->addWidget(m_outputSoundCbx, Qt::AlignLeft);

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

    //当扬声器状态发生变化，将switch设置为对应的状态
    connect(m_model, &SoundModel::isPortEnableChanged, this, [ = ](bool visible) {
        if (visible)
            m_sw->setChecked(true);
        else
            m_sw->setChecked(false);
    });

    connect(m_model, &SoundModel::setPortChanged, this, [ = ](const dcc::sound::Port  * port) {
        m_currentPort = port;
        Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->cardName());
    });

    auto ports = m_model->ports();
    for (auto port : ports) {
        addPort(port);
    }

    //连接switch点击信号，发送切换开/关扬声器的请求信号
    connect(m_sw, &SwitchWidget::checkedChanged, this, [ = ] {
        if(m_currentPort != nullptr)
            Q_EMIT m_model->requestSwitchSetEnable(m_currentPort->cardId(), m_currentPort->cardName(), m_sw->checked());
        else
            m_sw->setChecked(false);
    });



    connect(m_model, &SoundModel::portAdded, this, &SpeakerPage::addPort);

    connect(m_model, &SoundModel::portRemoved, this, &SpeakerPage::removePort);

    connect(m_outputSoundCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, [this](const int  idx) {
        auto temp = m_outputModel->index(idx, 0);
        this->requestSetPort(m_outputModel->data(temp, Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>());
    });

    initSlider();
}

void SpeakerPage::removePort(const QString &portId, const uint &cardId)
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

    rmFunc(m_outputModel);
}

void SpeakerPage::addPort(const dcc::sound::Port *port)
{
    if (port->Out == port->direction()) {
        DStandardItem *pi = new DStandardItem;
        pi->setText(port->name() + "(" + port->cardName() + ")");

        pi->setData(QVariant::fromValue<const dcc::sound::Port *>(port), Qt::WhatsThisPropertyRole);

        connect(port, &dcc::sound::Port::nameChanged, this, [ = ](const QString str) {
            pi->setText(str);
        });
        connect(port, &dcc::sound::Port::isActiveChanged, this, [ = ](bool isActive) {
            pi->setCheckState(isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
            if (isActive) {
                m_outputSoundCbx->comboBox()->setCurrentText(port->name() + "(" + port->cardName() + ")");
            }
        });

        m_outputModel->blockSignals(true);
        m_outputModel->appendRow(pi);
        m_outputModel->blockSignals(false);
        if (port->isActive()) {
            m_outputSoundCbx->comboBox()->setCurrentText(port->name());
            m_currentPort = port;
            Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->cardName());
        }
    }
}

void SpeakerPage::initSlider()
{
    //~ contents_path /sound/Speaker
    m_outputSlider = new TitledSliderItem(tr("Output Volume"), this);
    m_outputSlider->addBackground();
    m_outputSlider->setVisible(m_model->isPortEnable());
    m_speakSlider = m_outputSlider->slider();

    QStringList annotions;
    if (m_model->MaxUIVolume() > 1.0) {
        annotions << "0" << " " << "100" << "150";
        m_speakSlider->setAnnotations(annotions);
    } else {
        annotions << "0" << " " << "100";
        m_speakSlider->setAnnotations(annotions);
    }

    //初始化音量设置滚动条
    qDebug() << "max volume:" << m_model->MaxUIVolume();
    int maxRange = static_cast<int>(m_model->MaxUIVolume() * static_cast<int>(100.0f));
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
    int val = static_cast<int>(m_model->speakerVolume() * static_cast<int>(100.0f));
    if (val > maxRange) {
        val = maxRange;
    }
    m_speakSlider->setValue(val);
    m_outputSlider->setValueLiteral(QString::number(int(val)) + "%");
    m_speakSlider->setPageStep(1);

    //处理滑块位置变化的槽
    auto slotfunc1 = [ = ](int pos) {
        double vals = pos / 100.0;
        //滑块位置改变时，发送设置音量的信号
        Q_EMIT requestSetSpeakerVolume(vals);
    };
    //当点击滑槽时不会有，sliderMoved消息，用这个补
    connect(m_speakSlider, &DCCSlider::valueChanged, slotfunc1);
    //滑块移动消息处理
    connect(m_speakSlider, &DCCSlider::sliderMoved, slotfunc1);
    //当扬声器开/关时，显示/隐藏控件
    connect(m_model, &SoundModel::isPortEnableChanged, m_outputSlider, &TitledSliderItem::setVisible);
    //当底层数据改变后，更新滑动条显示的数据
    connect(m_model, &SoundModel::speakerVolumeChanged, this, [ = ](double v) {
        m_speakSlider->blockSignals(true);
        m_speakSlider->setValue(v * 100);
        m_speakSlider->blockSignals(false);
        m_outputSlider->setValueLiteral(QString::number(v * 100) + "%");
    });

    connect(m_model, &SoundModel::maxUIVolumeChanged, this, [ = ](double maxvalue) {
        m_speakSlider->setRange(0, static_cast<int>(maxvalue * 100));
        QStringList annotion;
        if (maxvalue > 1.0) {
            annotion << "0 " << "" << "100" << "150 ";
            qDebug() << m_outputSlider << annotion;
            m_outputSlider->slider()->setRightTicks(annotion);
        } else {
            annotion << "0 " << "" << "100";
            m_outputSlider->slider()->setRightTicks(annotion);
        }
        m_outputSlider->update();

        m_speakSlider->blockSignals(true);
        m_speakSlider->setValue(static_cast<int>(m_model->speakerVolume() * 100));
        m_speakSlider->blockSignals(false);
        m_outputSlider->setValueLiteral(QString::number(m_model->speakerVolume() * 100) + "%");
    });

    m_layout->insertWidget(3, m_outputSlider);

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
    vbWidget->setVisible(m_model->isPortEnable());
    m_layout->insertWidget(4, vbWidget);
    connect(volumeBoost, &SwitchWidget::checkedChanged, volumeBoostTip, &DTipLabel::setVisible);
    connect(m_model, &SoundModel::increaseVolumeChanged, volumeBoostTip, &DTipLabel::setVisible);
    connect(m_model, &SoundModel::isPortEnableChanged, vbWidget, &QWidget::setVisible);

    //~ contents_path /sound/Speaker
    auto balanceSlider = new TitledSliderItem(tr("Left/Right Balance"), this);
    balanceSlider->addBackground();
    balanceSlider->setVisible(m_model->isPortEnable());

    //信号处理与上面一致
    QStringList balanceList;
    balanceList << tr("Left") << " ";
    balanceList << tr("Right");
    DCCSlider *slider2 = balanceSlider->slider();
    slider2->setRange(-100, 100);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::TicksBelow);
    slider2->setTickInterval(1);
    slider2->setSliderPosition(static_cast<int>(m_model->speakerBalance() * 100 + 0.000001));
    slider2->setPageStep(1);
    balanceSlider->setAnnotations(balanceList);

    auto slotfunc2 = [ = ](int pos) {
        double value = pos / 100.0;
        Q_EMIT requestSetSpeakerBalance(value);
    };
    connect(slider2, &DCCSlider::valueChanged, slotfunc2);
    connect(slider2, &DCCSlider::sliderMoved, slotfunc2);
    connect(m_model, &SoundModel::isPortEnableChanged, balanceSlider, &TitledSliderItem::setVisible);
    connect(m_model, &SoundModel::speakerBalanceChanged, this, [ = ](double v) {
        slider2->blockSignals(true);
        slider2->setSliderPosition(static_cast<int>(v * 100 + 0.000001));
        slider2->blockSignals(false);
    });

    m_layout->insertWidget(5, balanceSlider);
}
