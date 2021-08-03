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
#include "window/gsettingwatcher.h"

#include <com_deepin_daemon_audio_source.h>

#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "types/audioport.h"

#include <DStandardItem>
#include <DStyle>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSvgRenderer>


using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;
using com::deepin::daemon::audio::Source;

Q_DECLARE_METATYPE(const dcc::sound::Port *)

MicrophonePage::MicrophonePage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_volumeBtn(nullptr)
    , m_waitTimerValue(0)
    , m_lastRmPortIndex(-1)
    , m_mute(false)
    , m_enablePort(false)
    , m_enable(true)
    , m_fristChangePort(true)
    , m_currentBluetoothPortStatus(true)
    , m_waitStatusChangeTimer(new QTimer (this))
    , m_waitCurrentPortRemove( new QTimer (this))
{
    const int titleLeftMargin = 8;
    TitleLabel *labelInput = new TitleLabel(tr("Input"));
    DFontSizeManager::instance()->bind(labelInput, DFontSizeManager::T5, QFont::DemiBold);
    labelInput->setContentsMargins(titleLeftMargin, 0, 0, 0);
    labelInput->setAlignment(Qt::AlignLeft | Qt::AlignTop);


    m_inputSoundCbx = new ComboxWidget(tr("Input Device"));

    TitleLabel *ndTitle = new TitleLabel(tr("Automatic Noise Suppression"));
    DFontSizeManager::instance()->bind(ndTitle, DFontSizeManager::T6);
    m_noiseReductionsw = new SwitchWidget(nullptr, ndTitle);
    m_noiseReductionsw->addBackground();

    m_inputModel  = new QStandardItemModel(m_inputSoundCbx->comboBox());
    m_inputSoundCbx->comboBox()->setModel(m_inputModel);

    m_layout->addWidget(labelInput);
    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_waitStatusChangeTimer->setSingleShot(true);
    m_waitCurrentPortRemove->setSingleShot(true);
    setLayout(m_layout);
}

MicrophonePage::~MicrophonePage()
{
    m_waitStatusChangeTimer->stop();
    m_waitCurrentPortRemove->stop();

#ifndef DCC_DISABLE_FEEDBACK
    if (m_feedbackSlider)
        m_feedbackSlider->disconnect(m_conn);
    m_feedbackSlider->deleteLater();
#endif

    GSettingWatcher::instance()->erase("soundInputSlider");
    GSettingWatcher::instance()->erase("soundFeedbackSlider");
    GSettingWatcher::instance()->erase("soundNoiseReduce");
}

/**当用户进入扬声器端口手动切换蓝牙输出端口后，再进入麦克风页面时
 * 会有默认输入端口路径为空或者指定路径下激活端口为空的情况，
 */
void MicrophonePage::resetUi()
{
    QDBusInterface interface("com.deepin.daemon.Audio", "/com/deepin/daemon/Audio", "com.deepin.daemon.Audio", QDBusConnection::sessionBus(), this);
    QDBusObjectPath defaultPath = interface.property("DefaultSource").value<QDBusObjectPath>();
    if (defaultPath.path() == "/") //路径为空
        m_inputSoundCbx->comboBox()->setCurrentIndex(-1);
    else {
        Source defaultSourcer("com.deepin.daemon.Audio", defaultPath.path(), QDBusConnection::sessionBus(), this);
        AudioPort port = defaultSourcer.activePort();
        if (port.name.isEmpty() || port.description.isEmpty()) {
            m_inputSoundCbx->comboBox()->setCurrentIndex(-1);
        }

        showDevice();
    }
}

void MicrophonePage::setModel(SoundModel *model)
{
    m_model = model;
    m_waitTimerValue = m_model->currentWaitSoundReceiptTime();

    //监听消息设置是否可用
    connect(m_model, &SoundModel::isPortEnableChanged, this, [ = ](bool enable) {
        m_enable = enable;
        //启用端口后需要再判断是否启用成功后，再设置为默认端口，但因为设置端口后会有端口是否启用的状态判断，
        //导致进入死循环，所以添加判断值，判断是否是启用或禁用端口类型的操作，若是，则设置默认端口
        if (enable && m_enablePort) {
            QModelIndex index = m_inputSoundCbx->comboBox()->view()->currentIndex();
            if (index.isValid())
                Q_EMIT requestSetPort(m_inputModel->data(index, Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>());
        }
        showDevice();
    });
    //发送查询请求消息看是否可用
    connect(m_model, &SoundModel::setPortChanged, this, [ = ](const dcc::sound::Port  * port) {
        m_enablePort = false;
        Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->id());
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

    connect(m_model, &SoundModel::portAdded, this, &MicrophonePage::addPort);
    connect(m_model, &SoundModel::portRemoved, this, &MicrophonePage::removePort);
    connect(m_model, &SoundModel::soundDeviceStatusChanged, this, &MicrophonePage::changeComboxStatus);
    connect(m_inputSoundCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this, &MicrophonePage::changeComboxIndex);

    connect(m_noiseReductionsw, &SwitchWidget::checkedChanged, this, &MicrophonePage::requestReduceNoise);
    connect(m_model, &SoundModel::reduceNoiseChanged, m_noiseReductionsw, &SwitchWidget::setChecked);
    connect(m_model, &SoundModel::microphoneOnChanged, this, [ = ](bool flag) {
        m_mute = flag;
        refreshIcon();
    });

    initSlider();
    initCombox();
}

void MicrophonePage::removePort(const QString &portId, const uint &cardId)
{
    if ((m_inputModel->rowCount() != m_model->ports().size()) && (m_lastRmPortIndex != -1)) {
        m_inputModel->removeRow(m_lastRmPortIndex);
        m_lastRmPortIndex = -1;
    }

    for (int i = 0; i < m_inputModel->rowCount(); ++i) {
        auto item = m_inputModel->item(i);
        auto port = item->data(Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>();
        if (port->id() == portId && cardId == port->cardId()) {
            m_inputSoundCbx->comboBox()->hidePopup();
            if (m_currentPort->id() == portId && m_currentPort->cardId() == cardId)
                m_currentPort = nullptr;

            // 当只有一个端口 拔出端口后直接移除，不进行延迟置灰操作
            if (m_inputModel->rowCount() == 1) {
                m_inputModel->removeRow(i);
                showDevice();
                return;
            }
            m_lastRmPortIndex = i;
        }
    }

    if (m_lastRmPortIndex == -1)
        return;

    m_inputSoundCbx->blockSignals(true);
    m_waitCurrentPortRemove->disconnect();
    connect(m_waitCurrentPortRemove, &QTimer::timeout, this, [=](){
        m_inputSoundCbx->blockSignals(false);
        m_inputModel->removeRow(m_lastRmPortIndex);
        m_lastRmPortIndex = -1;
    });
    m_waitCurrentPortRemove->start(m_waitTimerValue);
    changeComboxStatus();
    showDevice();
}

void MicrophonePage::changeComboxIndex(const int idx)
{
    if (idx < 0)
        return;

    auto tFunc = [this](const int tmpIdx) {
        auto temp = m_inputModel->index(tmpIdx, 0);
        const dcc::sound::Port *port = m_inputModel->data(temp, Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>();
        this->requestSetPort(port);
        qDebug() << "default source index change, currentTerxt:" << m_inputSoundCbx->comboBox()->itemText(tmpIdx);
    };

    tFunc(idx);
    changeComboxStatus();

    showDevice();
}

void MicrophonePage::changeComboxStatus()
{
    showWaitSoundPortStatus(false);
    m_waitStatusChangeTimer->disconnect();
    connect(m_waitStatusChangeTimer, &QTimer::timeout, this, [=](){
        refreshActivePortShow(m_currentPort);
        showWaitSoundPortStatus(true);
    });
    m_waitStatusChangeTimer->start(m_waitTimerValue);
}

void MicrophonePage::refreshActivePortShow(const dcc::sound::Port *port)
{
    if (port && port->isActive()) {
        m_inputSoundCbx->comboBox()->setCurrentText(port->name() + "(" + port->cardName() + ")");
        m_currentBluetoothPortStatus = port->isBluetoothPort();
        showDevice();
    }
}

void MicrophonePage::addPort(const dcc::sound::Port *port)
{
    if (!port->isEnabled())
        return;

    if (port->In == port->direction()) {
        m_enable = port->isEnabled();

        DStandardItem *pi = new DStandardItem;
        pi->setText(port->name() + "(" + port->cardName() + ")");
        pi->setData(QVariant::fromValue<const dcc::sound::Port *>(port), Qt::WhatsThisPropertyRole);

        connect(port, &dcc::sound::Port::nameChanged, this, [ = ](const QString str) {
            pi->setText(str);
        });
        connect(port, &dcc::sound::Port::isInputActiveChanged, this, [ = ](bool isActive) {
            // 若关闭设备 此时pi为空
            m_currentPort = port;
            if (pi) {
                pi->setCheckState(isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
                if (isActive)
                    changeComboxStatus();
            }
        });
        m_inputSoundCbx->comboBox()->hidePopup();
        m_inputModel->appendRow(pi);
        if (port->isActive()) {
            m_currentPort = port;
            refreshActivePortShow(m_currentPort);
            Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->id());
        }
        showDevice();
    }
}

void MicrophonePage::toggleMute()
{
    Q_EMIT requestMute();
}

void MicrophonePage::initSlider()
{
    m_inputSlider = new TitledSliderItem(tr("Input Volume"));
    m_inputSlider->addBackground();
    m_layout->insertWidget(2, m_inputSlider);

    m_volumeBtn = new SoundLabel(this);
    QGridLayout *gridLayout = dynamic_cast<QGridLayout *>(m_inputSlider->slider()->layout());
    if (gridLayout) {
        gridLayout->addWidget(m_volumeBtn, 1, 0, Qt::AlignVCenter);
    }
    m_volumeBtn->setAccessibleName("volume-button");
    m_volumeBtn->setFixedSize(ICON_SIZE, ICON_SIZE);
    m_volumeBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));

    DCCSlider *slider = m_inputSlider->slider();
    slider->setRange(0, 100);
    slider->setType(DCCSlider::Vernier);
    slider->setTickPosition(QSlider::NoTicks);
    auto icon_high = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeHighElement);
    slider->setRightIcon(icon_high);
    slider->setIconSize(QSize(24, 24));
    slider->setTickInterval(1);
    slider->setSliderPosition(int(m_model->microphoneVolume() * 100));
    slider->setPageStep(1);

    auto slotfunc1 = [ = ](int pos) {
        double val = pos / 100.0;
        Q_EMIT requestSetMicrophoneVolume(val);
        Q_EMIT requestMute(false);
    };
    int val = static_cast<int>(m_model->microphoneVolume() * static_cast<int>(100.0f));
    slider->setValue(val);
    m_inputSlider->setValueLiteral(QString::number(m_model->microphoneVolume() * 100) + "%");
    connect(slider, &DCCSlider::valueChanged, this, slotfunc1);
    connect(slider, &DCCSlider::sliderMoved, this, slotfunc1);
    connect(m_model, &SoundModel::microphoneVolumeChanged, this, [ = ](double v) {
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(v * 100));
        slider->setSliderPosition(static_cast<int>(v * 100));
        slider->blockSignals(false);
        m_inputSlider->setValueLiteral(QString::number(v * 100) + "%");
    });
    connect(m_volumeBtn, &SoundLabel::clicked, this, &MicrophonePage::toggleMute);

#ifndef DCC_DISABLE_FEEDBACK
    m_feedbackSlider = (new TitledSliderItem(tr("Input Level")));
    m_feedbackSlider->addBackground();
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

    connect(m_model, &SoundModel::isPortEnableChanged, m_noiseReductionsw, &ComboxWidget::setVisible);
    m_conn = connect(m_model, &SoundModel::microphoneFeedbackChanged, [ = ](double vol2) {
        int pos = GSettingWatcher::instance()->getStatus("soundFeedbackSlider") == "Disabled" ? 0 : (int(vol2 * 100));
        slider2->setSliderPosition(pos);
    });
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MicrophonePage::refreshIcon);
    connect(qApp, &DApplication::iconThemeChanged, this, &MicrophonePage::refreshIcon);
    m_layout->insertWidget(3, m_feedbackSlider);

    // 使用GSettings来控制显示状态
    GSettingWatcher::instance()->bind("soundInputSlider", m_inputSlider);
    GSettingWatcher::instance()->bind("soundFeedbackSlider", m_feedbackSlider);
    GSettingWatcher::instance()->bind("soundNoiseReduce", m_noiseReductionsw);

    refreshIcon();
    showDevice();
}

void MicrophonePage::initCombox()
{
    SettingsGroup *inputSoundsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    inputSoundsGrp->getLayout()->setContentsMargins(ThirdPageCmbMargins);
    inputSoundsGrp->appendItem(m_inputSoundCbx);

    m_layout->addWidget(m_noiseReductionsw);
    m_layout->addWidget(inputSoundsGrp);
    m_layout->setSpacing(10);
    m_layout->addStretch(10);
#else
}

void MicrophonePage::initCombox()
{
#endif
    //放到宏外面修复sw架构下音频布局异常的问题
    m_layout->addStretch(10);
}

void MicrophonePage::refreshIcon()
{
    if (m_mute) {
        m_volumeBtn->setIcon(qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeMutedElement));
    } else {
        m_volumeBtn->setIcon(qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeLowElement));
    }
}

void MicrophonePage::showWaitSoundPortStatus(bool showStatus)
{
    m_inputSoundCbx->setEnabled(showStatus);
    m_noiseReductionsw->setEnabled(showStatus);
}

/**
 * @brief MicrophonePage::showDevice
 * 当默认设备为空时隐藏设备信息
 * 当无设备时，不显示设备信息
 * 当有多个设备，且未禁用时，显示设备信息
 * 跟随设备管理
 */
void MicrophonePage::showDevice()
{
    if (!m_feedbackSlider || !m_inputSlider || !m_noiseReductionsw)
        return;

    if (1 > m_inputModel->rowCount())
        setDeviceVisible(false);
    else {
        setDeviceVisible(m_enable);
    }
}

void MicrophonePage::setDeviceVisible(bool visable)
{
    if (visable) {
        if (GSettingWatcher::instance()->getStatus("soundFeedbackSlider") != "Hidden")
            m_feedbackSlider->show();
        if (GSettingWatcher::instance()->getStatus("soundInputSlider") != "Hidden")
            m_inputSlider->show();
        if (GSettingWatcher::instance()->getStatus("soundNoiseReduce") != "Hidden")
            m_noiseReductionsw->setVisible(!m_currentBluetoothPortStatus);
    } else {
        m_feedbackSlider->hide();
        m_inputSlider->hide();
        m_noiseReductionsw->hide();
    }
}
