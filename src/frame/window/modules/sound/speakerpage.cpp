// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "speakerpage.h"
#include "modules/sound/soundmodel.h"
#include "widgets/switchwidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/titlelabel.h"
#include "widgets/settingsgroup.h"

#include <DStyle>
#include <DTipLabel>
#include <DFontSizeManager>
#include <DStandardItem>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDebug>
#include <QSvgRenderer>

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
    , m_vbWidget(nullptr)
    , m_balanceSlider(nullptr)
    , m_waitTimerValue(0)
    , m_lastRmPortIndex(-1)
    , m_balance(true)
    , m_volumeBtn(nullptr)
    , m_mute(false)
    , m_enablePort(false)
    , m_fristChangePort(true)
    , m_fristStatusChangePort(true)
    , m_waitStatusChangeTimer(new QTimer (this))
{
    const int titleLeftMargin = 8;

    //~ contents_path /sound/Output
    //~ child_page Output
    TitleLabel *labelOutput = new TitleLabel(tr("Output"));
    DFontSizeManager::instance()->bind(labelOutput, DFontSizeManager::T5, QFont::DemiBold);
    labelOutput->setContentsMargins(titleLeftMargin, 0, 0, 0);
    labelOutput->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    //~ contents_path /sound/Output
    //~ child_page Output
    m_outputSoundCbx = new ComboxWidget(tr("Output Device"));
    m_outputModel  = new QStandardItemModel(m_outputSoundCbx->comboBox());
    m_outputSoundCbx->comboBox()->setModel(m_outputModel);
    m_outputSoundCbx->comboBox()->setAccessibleName("outputSoundCbx");

    m_outputSoundsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    //~ contents_path /sound/Output
    //~ child_page Output
    m_blueSoundCbx = new ComboxWidget(tr("Mode"));
    m_blueSoundCbx->comboBox()->setAccessibleName("blueSoundCbx");

    m_layout->addWidget(labelOutput);
    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_waitStatusChangeTimer->setSingleShot(true);
    connect(m_waitStatusChangeTimer, &QTimer::timeout, this, [ = ] {
        refreshActivePortShow(m_currentPort);
        showWaitSoundPortStatus(true);
    });
    setLayout(m_layout);
}

SpeakerPage::~SpeakerPage()
{
    m_waitStatusChangeTimer->stop();
    GSettingWatcher::instance()->erase("soundOutputSlider");
    GSettingWatcher::instance()->erase("soundVolumeBoost");
    GSettingWatcher::instance()->erase("soundBalanceSlider");
}

void SpeakerPage::setModel(dcc::sound::SoundModel *model)
{
    m_model = model;
    m_waitTimerValue = m_model->currentWaitSoundReceiptTime();

    //当扬声器状态发生变化，更新设备信息显示状态
    connect(m_model, &SoundModel::isPortEnableChanged, this, [ = ](bool visible) {
        Q_UNUSED(visible);
        //启用端口后需要再判断是否启用成功后，再设置为默认端口，但因为设置端口后会有端口是否启用的状态判断，
        //导致进入死循环，所以添加判断值，判断是否是启用或禁用端口类型的操作，若是，则设置默认端口
        if (m_enablePort) {
            QModelIndex index = m_outputSoundCbx->comboBox()->view()->currentIndex();
            if (index.isValid()) {
                Q_EMIT requestSetPort(m_outputModel->data(index, Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>());
            }
        }
        showDevice();
    });

    connect(m_model, &SoundModel::setPortChanged, this, [ = ](const dcc::sound::Port  * port) {
        m_enablePort = false;

        Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->id());//设置端口后，发送信号，判断该端口是否需要禁用
    });

    auto ports = m_model->ports();
    for (auto port : ports) {
        addPort(port);
    }

    connect(m_outputSoundCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &SpeakerPage::changeComboxIndex, Qt::QueuedConnection);
    connect(m_model, &SoundModel::bluetoothModeOptsChanged, this, [ = ](const QStringList bluetoothModeOpts){
        if (m_bluetoothModeOpts != bluetoothModeOpts) {
            m_bluetoothModeOpts = bluetoothModeOpts;
            m_blueSoundCbx->comboBox()->clear(); // 先清除避免重复
            m_blueSoundCbx->comboBox()->addItems(m_bluetoothModeOpts);
        }
    });
    connect(m_model, &SoundModel::portRemoved, this, &SpeakerPage::removePort);
    connect(m_model, &SoundModel::portAdded, this, &SpeakerPage::addPort);
    connect(m_model, &SoundModel::soundDeviceStatusChanged, this, &SpeakerPage::changeComboxStatus, Qt::UniqueConnection);
    connect(m_model, &SoundModel::bluetoothModeChanged, this, [ = ](const QString &mode) {
        m_blueSoundCbx->setCurrentText(mode);
        m_balance = !mode.contains("headset");
        changeComboxStatus();
    });
    connect(m_model, &SoundModel::speakerOnChanged, this, [ = ](bool flag) {
        m_mute = flag;
        refreshIcon();
    });
    connect(m_model, &SoundModel::speakerNameChanged, this, [ this ](const QString &name) {
        m_speakerName = name;
        showDevice();
    });

    initSlider();
    initCombox();
}

void SpeakerPage::removePort(const QString &portId, const uint &cardId, const dcc::sound::Port::Direction &direction)
{
    if (direction != dcc::sound::Port::Direction::Out)
        return;

    // 先阻塞m_outputSoundCbx切换信号，避免移除当前端口时被动触发切换端口信号，而新的当前端口由是后端确定
    m_outputSoundCbx->blockSignals(true);
    m_outputSoundCbx->comboBox()->hidePopup();

    // 查找移除的端口直接从model中移除，如果是当前端口直接设置为空，新的当前端口由后端确定，控制中心响应信号设置当前端口
    for (int i = 0; i < m_outputModel->rowCount(); i++) {
        auto item = m_outputModel->item(i);
        auto port = item->data(Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>();
        if (port && port->id() == portId && cardId == port->cardId()) {
            m_outputModel->removeRow(i);
            if (m_currentPort && m_currentPort->id() == portId && m_currentPort->cardId() == cardId) {
                m_currentPort = nullptr;
            }
            break;
        }
    }

    changeComboxStatus();
    showDevice();
    m_outputSoundCbx->blockSignals(false);
}

void SpeakerPage::changeComboxIndex(const int idx)
{
    if (idx < 0)
        return;

    auto tFunc = [this](const int tmpIdx){
        auto temp = m_outputModel->index(tmpIdx, 0);
        this->requestSetPort(m_outputModel->data(temp, Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>());
        qDebug() << "default sink index change, currentTerxt:" << m_outputSoundCbx->comboBox()->itemText(tmpIdx);
    };

    tFunc(idx);
    changeComboxStatus();

    showDevice();
}

void SpeakerPage::changeComboxStatus()
{
    showWaitSoundPortStatus(false);
    if (m_fristStatusChangePort) {
        refreshActivePortShow(m_currentPort);
        showWaitSoundPortStatus(true);
        m_fristStatusChangePort = false;
    } else {
        m_waitStatusChangeTimer->stop();
    }
    m_waitStatusChangeTimer->start(m_waitTimerValue);
    showDevice();
}

void SpeakerPage::clickLeftButton()
{
    Q_EMIT requestMute();
}

void SpeakerPage::changeBluetoothMode(const int idx)
{
    this->requstBluetoothMode(m_blueSoundCbx->comboBox()->itemText(idx));
}

void SpeakerPage::refreshActivePortShow(const dcc::sound::Port *port)
{
    if (port && port->isActive()) {
        // 端口激活和未激活时，carname内容不一致，此时无法通过settext匹配选择项，改为通过查找id来匹配
        for (int i = 0; i < m_outputSoundCbx->comboBox()->count(); i++) {
            auto tmpPort = m_outputModel->item(i)->data(Qt::WhatsThisPropertyRole).value<const dcc::sound::Port *>();

            if (tmpPort != nullptr && tmpPort->id() == port->id() && tmpPort->cardName() ==  port->cardName()) {
                m_outputSoundCbx->comboBox()->blockSignals(true);
                m_outputSoundCbx->comboBox()->setCurrentIndex(i);
                m_outputSoundCbx->comboBox()->blockSignals(false);
                break;
            }
        }

        setBlueModeVisible(port->isBluetoothPort() && (m_outputModel->rowCount() > 0));
    } else {
        setBlueModeVisible(false);
    }
}

bool SpeakerPage::hasVirtualSink() {
    return m_model->ports().isEmpty() && !m_speakerName.startsWith("auto_null");
}

void SpeakerPage::addPort(const dcc::sound::Port *port)
{
    if (dcc::sound::Port::Out == port->direction()) {
        qDebug() << "SpeakerPage::addPort" << port->name();
        DStandardItem *pi = new DStandardItem;
        pi->setText(port->name() + "(" + port->cardName() + ")");
        pi->setData(QVariant::fromValue<const dcc::sound::Port *>(port), Qt::WhatsThisPropertyRole);

        connect(port, &dcc::sound::Port::nameChanged, this, [ = ](const QString str) {
            pi->setText(str + "(" + port->cardName() + ")");
        });

        connect(port, &dcc::sound::Port::cardNameChanged, this, [ = ](const QString str) {
            pi->setText(port->name() + "(" + str + ")");
        });

        connect(port, &dcc::sound::Port::isOutputActiveChanged, this, [ = ](bool isActive) {
            if (pi)
                pi->setCheckState(isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
            if (isActive) {
                m_currentPort = port;
                changeComboxStatus();
            }
        });
        connect(port, &dcc::sound::Port::currentPortEnabled, this, [ = ](bool isEnable) {
            int index = m_outputSoundCbx->comboBox()->findData(port);
            // 若端口可用 且没有添加
            if (isEnable && (index == -1) && pi) {
                m_outputModel->appendRow(pi);
                if (port == m_currentPort) {
                    refreshActivePortShow(m_currentPort);
                }
                showDevice();
            }

            if (!isEnable && (index != -1)) {
                m_outputModel->removeRow(index);
                showDevice();
            }
        });

        m_outputSoundCbx->comboBox()->hidePopup();
        if (port->isEnabled())
            m_outputModel->appendRow(pi);
        if (port->isActive()) {
            m_currentPort = port;
            refreshActivePortShow(m_currentPort);
            Q_EMIT m_model->requestSwitchEnable(port->cardId(), port->id());
        }
        showDevice();
    }
}

void SpeakerPage::initSlider()
{
    //~ contents_path /sound/Output
    //~ child_page Output
    m_outputSlider = new TitledSliderItem(tr("Output Volume"), this);
    m_outputSlider->addBackground();
    m_speakSlider = m_outputSlider->slider();

    if (m_model->MaxUIVolume() > 1.0) {
        m_speakSlider->setSeparateValue(100);
    } else {
        m_speakSlider->setSeparateValue(0);
    }

    //初始化音量设置滚动条
    qDebug() << "max volume:" << m_model->MaxUIVolume();
    int maxRange = static_cast<int>(m_model->MaxUIVolume() * 100.0f + 0.000001);
    m_speakSlider->setRange(0, maxRange);
    m_speakSlider->setType(DCCSlider::Vernier);
    m_speakSlider->setTickPosition(QSlider::NoTicks);

    m_volumeBtn = new SoundLabel(this);
    QGridLayout *gridLayout = dynamic_cast<QGridLayout *>(m_outputSlider->slider()->layout());
    if (gridLayout) {
        gridLayout->addWidget(m_volumeBtn, 1, 0, Qt::AlignVCenter);
    }
    m_volumeBtn->setAccessibleName("volume-button");
    m_volumeBtn->setFixedSize(ICON_SIZE, ICON_SIZE);
    m_volumeBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    //从DStyle 中获取标准图标
    auto icon_high = qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeHighElement);
    m_outputSlider->setRightIcon(icon_high);
    m_outputSlider->setIconSize(QSize(24, 24));

    m_speakSlider->setTickInterval(1);
    qDebug() << "speaker volume:" << m_model->speakerVolume();
    int val = static_cast<int>(m_model->speakerVolume() * 100.0f + 0.000001);
    if (val > maxRange) {
        val = maxRange;
    }
    m_speakSlider->setValue(val);
    m_outputSlider->setValueLiteral(QString::number(int(val)) + "%");
    m_speakSlider->setPageStep(1);

    //处理滑块位置变化的槽
    auto slotfunc1 = [ = ](int pos) {
        m_outputSlider->slider()->qtSlider()->setSingleStep(1);
        double vals = pos / 100.0;
        //滑块位置改变时，发送设置音量的信号
        Q_EMIT requestSetSpeakerVolume(vals);
        Q_EMIT requestMute(false);
    };
    //当点击滑槽时不会有，sliderMoved消息，用这个补
    connect(m_speakSlider, &DCCSlider::valueChanged, slotfunc1);
    //滑块移动消息处理
    connect(m_speakSlider, &DCCSlider::sliderMoved, slotfunc1);
    //当扬声器开/关时，显示/隐藏控件
    //当底层数据改变后，更新滑动条显示的数据
    connect(m_model, &SoundModel::speakerVolumeChanged, this, [ = ](double v) {
        m_speakSlider->blockSignals(true);
        m_speakSlider->setValue(v * 100 + 0.000001);
        m_speakSlider->blockSignals(false);
        m_outputSlider->setValueLiteral(QString::number((int)(v * 100 + 0.000001)) + "%");
    });

    connect(m_model, &SoundModel::maxUIVolumeChanged, this, [ = ](double maxvalue) {
        m_speakSlider->setRange(0, static_cast<int>(maxvalue * 100 + 0.000001));
        if (maxvalue > 1.0) {
            qDebug() << m_outputSlider << maxvalue;
            m_speakSlider->setSeparateValue(100);
        } else {
            m_speakSlider->setSeparateValue(0);
        }
        m_outputSlider->update();

        m_speakSlider->blockSignals(true);
        m_speakSlider->setValue(static_cast<int>(m_model->speakerVolume() * 100 + 0.000001));
        m_speakSlider->blockSignals(false);
        m_outputSlider->setValueLiteral(QString::number((int)(m_model->speakerVolume() * 100 + 0.000001))+ "%");
    });
    connect(m_volumeBtn, &SoundLabel::clicked, this, &SpeakerPage::clickLeftButton);
    m_layout->insertWidget(2, m_outputSlider);

    //音量增强
    auto hlayout = new QVBoxLayout();
    auto volumeBoost = new SwitchWidget(this);
    volumeBoost->addBackground();
    volumeBoost->setChecked(m_model->isIncreaseVolume());

    //~ contents_path /sound/Output
    //~ child_page Output
    volumeBoost->setTitle(tr("Volume Boost"));
    connect(m_model, &SoundModel::increaseVolumeChanged, volumeBoost, &SwitchWidget::setChecked);
    connect(volumeBoost, &SwitchWidget::checkedChanged, this, &SpeakerPage::requestIncreaseVolume);
    hlayout->addWidget(volumeBoost);
    hlayout->setContentsMargins(0, 0, 0, 0);

    //下方提示
    auto volumeBoostTip = new DTipLabel(tr("If the volume is louder than 100%, it may distort audio and be harmful to your speaker"), this);
    volumeBoostTip->setWordWrap(true);
    volumeBoostTip->setAlignment(Qt::AlignLeft);
    volumeBoostTip->setContentsMargins(10, 0, 0, 0);
    hlayout->addWidget(volumeBoostTip);
    m_vbWidget = new QWidget(this);
    m_vbWidget->setAccessibleName("SpeakerPage_vbWidget");
    m_vbWidget->setLayout(hlayout);
    m_vbWidget->setVisible(m_model->isPortEnable());
    m_layout->insertWidget(3, m_vbWidget);
    m_layout->addWidget(m_vbWidget);

    //~ contents_path /sound/Output
    //~ child_page Output
    m_balanceSlider = new TitledSliderItem(tr("Left/Right Balance"), this);
    m_balanceSlider->addBackground();

    //信号处理与上面一致
    QStringList balanceList;
    balanceList << tr("Left") << " ";
    balanceList << tr("Right");
    DCCSlider *slider2 = m_balanceSlider->slider();
    slider2->setRange(-100, 100);
    slider2->setType(DCCSlider::Vernier);
    slider2->setTickPosition(QSlider::TicksBelow);
    slider2->setTickInterval(1);
    slider2->setSliderPosition(static_cast<int>(m_model->speakerBalance() * 100 + 0.000001));
    slider2->setPageStep(1);
    m_balanceSlider->setAnnotations(balanceList);
    m_balance = !(m_model->currentBluetoothAudioMode().contains("headset"));

    auto slotfunc2 = [ = ](int pos) {
        double value = pos / 100.0;
        Q_EMIT requestSetSpeakerBalance(value);
    };
    connect(slider2, &DCCSlider::valueChanged, slotfunc2);
    connect(slider2, &DCCSlider::sliderMoved, slotfunc2);
    connect(m_model, &SoundModel::speakerBalanceChanged, this, [ = ](double v) {
        slider2->blockSignals(true);
        slider2->setSliderPosition(static_cast<int>(v * 100 + 0.000001));
        slider2->blockSignals(false);
    });
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &SpeakerPage::refreshIcon);
    connect(qApp, &DApplication::iconThemeChanged, this, &SpeakerPage::refreshIcon);

    m_layout->insertWidget(4, m_balanceSlider);

    // 使用GSettings来控制显示状态
    GSettingWatcher::instance()->bind("soundOutputSlider", m_outputSlider);
    GSettingWatcher::instance()->bind("soundVolumeBoost", volumeBoost);
    GSettingWatcher::instance()->bind("soundVolumeBoost", volumeBoostTip);
    GSettingWatcher::instance()->bind("soundBalanceSlider", m_balanceSlider);

    refreshIcon();
    showDevice();
}

void SpeakerPage::initCombox()
{
    m_blueSoundCbx->comboBox()->addItems(m_model->bluetoothAudioModeOpts());
    m_blueSoundCbx->comboBox()->setCurrentText(m_model->currentBluetoothAudioMode());
    connect(m_blueSoundCbx->comboBox(), static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &SpeakerPage::changeBluetoothMode);

    m_outputSoundsGrp->getLayout()->setContentsMargins(ThirdPageCmbMargins);
    m_outputSoundsGrp->appendItem(m_outputSoundCbx);
    m_outputSoundsGrp->appendItem(m_blueSoundCbx);

    if (m_outputSoundsGrp->layout())
        m_outputSoundsGrp->layout()->setContentsMargins(ThirdPageCmbMargins);

    m_layout->addWidget(m_outputSoundsGrp);
    m_layout->setSpacing(10);
    m_layout->addStretch(10);
}

void SpeakerPage::refreshIcon()
{
    if (m_mute) {
        m_volumeBtn->setIcon(qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeMutedElement));
    } else {
        m_volumeBtn->setIcon(qobject_cast<DStyle *>(style())->standardIcon(DStyle::SP_MediaVolumeLowElement));
    }
}

void SpeakerPage::showWaitSoundPortStatus(bool showStatus)
{
    if (!m_currentPort || !m_currentPort->isBluetoothPort() || m_model->currentBluetoothAudioMode().isEmpty()) {
        m_blueSoundCbx->setVisible(false);
    }

    m_outputSoundCbx->setEnabled(showStatus);
    m_blueSoundCbx->setEnabled(showStatus);
}

/**
 * @brief SpeakerPage::showDevice
 * 当无设备时，不显示设备信息
 * 当有多个设备，且未禁用时，显示设备信息
 * 跟随设备管理
 */
void SpeakerPage::showDevice()
{
    if (!m_speakSlider || !m_vbWidget || !m_balanceSlider || !m_outputSlider)
        return;

    // 支持云平台无端口设备的显示
    if (1 > m_outputModel->rowCount() && !hasVirtualSink()){
        setDeviceVisible(false);
        setBlueModeVisible(false);
    } else
        setDeviceVisible(true);

    // 云平台关闭输出设备和蓝牙模式
    m_outputSoundCbx->setVisible(!hasVirtualSink());
    if (hasVirtualSink())
        m_blueSoundCbx->setVisible(false);
}

void SpeakerPage::setDeviceVisible(bool visible)
{
    if (visible) {
        m_speakSlider->show();
        m_vbWidget->show();
        if (GSettingWatcher::instance()->getStatus("soundBalanceSlider") != "Hidden")
            m_balanceSlider->setVisible(m_balance);
        if (GSettingWatcher::instance()->getStatus("soundOutputSlider") != "Hidden")
            m_outputSlider->show();
    } else {
        m_speakSlider->hide();
        m_vbWidget->hide();
        m_balanceSlider->hide();
        m_outputSlider->hide();
    }
}

void SpeakerPage::setBlueModeVisible(bool visible)
{
    // 模式的状态应该跟随 蓝牙端口显示的时机
    m_blueSoundCbx->setVisible(visible);
    if (visible)
        m_blueSoundCbx->comboBox()->setCurrentText(m_model->currentBluetoothAudioMode());

}
