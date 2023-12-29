//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "microphonepage.h"
#include "soundmodel.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/comboxwidget.h"
#include "widgets/settingsgroup.h"
#include "audioport.h"

#include <DStandardItem>
#include <DStyle>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DApplication>
#include <QDBusPendingReply>
#include <DIconTheme>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDBusInterface>
#include <sounddbusproxy.h>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

Q_DECLARE_METATYPE(const Port *)

MicrophonePage::MicrophonePage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_volumeBtn(nullptr)
    , m_waitTimerValue(0)
    , m_lastRmPortIndex(-1)
    , m_enablePort(false)
    , m_fristChangePort(true)
    , m_currentBluetoothPortStatus(true)
    , m_waitStatusChangeTimer(new QTimer(this))
{
    m_inputSoundCbx = new ComboxWidget(tr("Input Device"));
    m_inputSoundCbx->comboBox()->setAccessibleName("inputSoundCbx");

    m_noiseReductionsw = new SwitchWidget(tr("Automatic Noise Suppression"), this);
    m_noiseReductionsw->addBackground();

    m_inputModel  = new QStandardItemModel(m_inputSoundCbx->comboBox());
    m_inputSoundCbx->comboBox()->setModel(m_inputModel);

    m_layout->setContentsMargins(0, 0, 0, 10);
    // TODO: 配置 DCONFIG
    connect(m_waitStatusChangeTimer, &QTimer::timeout, [this](){
        refreshActivePortShow(m_currentPort);
        showWaitSoundPortStatus(true);
    });
    m_waitStatusChangeTimer->setSingleShot(true);
    m_waitStatusChangeTimer->start();

    setLayout(m_layout);
}

MicrophonePage::~MicrophonePage()
{
    m_waitStatusChangeTimer->stop();

#ifndef DCC_DISABLE_FEEDBACK
    if (m_feedbackSlider)
        m_feedbackSlider->disconnect(m_conn);
    m_feedbackSlider->deleteLater();
#endif

}

/**当用户进入扬声器端口手动切换蓝牙输出端口后，再进入麦克风页面时
 * 会有默认输入端口路径为空或者指定路径下激活端口为空的情况，
 */
void MicrophonePage::resetUi()
{
    QDBusInterface interface("org.deepin.dde.Audio1", "/org/deepin/dde/Audio1", "org.deepin.dde.Audio1", QDBusConnection::sessionBus(), this);
    QDBusObjectPath defaultPath = interface.property("DefaultSource").value<QDBusObjectPath>();
    if (defaultPath.path() == "/" || defaultPath.path().isEmpty()) //路径为空
        m_inputSoundCbx->comboBox()->setCurrentIndex(-1);
    else {
        QDBusInterface *defaultSource = new QDBusInterface("org.deepin.dde.Audio1", defaultPath.path(), "org.deepin.dde.Audio1.Source", QDBusConnection::sessionBus(), this);
        AudioPort port = QDBusPendingReply<AudioPort>(defaultSource->asyncCall(QStringLiteral("ActivePort")));
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
        //启用端口后需要再判断是否启用成功后，再设置为默认端口，但因为设置端口后会有端口是否启用的状态判断，
        //导致进入死循环，所以添加判断值，判断是否是启用或禁用端口类型的操作，若是，则设置默认端口
        if (enable && m_enablePort) {
            QModelIndex index = m_inputSoundCbx->comboBox()->view()->currentIndex();
            if (index.isValid())
                Q_EMIT requestSetPort(m_inputModel->data(index, Qt::WhatsThisPropertyRole).value<const Port *>());
        }
        showDevice();
    });
    //发送查询请求消息看是否可用
    connect(m_model, &SoundModel::setPortChanged, this, [ = ](const Port  * port) {
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
        Q_UNUSED(flag)
        refreshIcon();
    });

    initSlider();
    initCombox();
}

void MicrophonePage::removePort(const QString &portId, const uint &cardId, const Port::Direction &direction)
{
    if (direction != Port::Direction::In)
        return;

    // 先阻塞m_outputSoundCbx切换信号，避免移除当前端口时被动触发切换当前端口信号，而新的当前端口由是后端确定
    m_inputSoundCbx->blockSignals(true);
    m_inputSoundCbx->comboBox()->hidePopup();

    // 查找移除的端口直接从model中移除，如果是当前端口直接设置为空，新的当前端口由后端确定，控制中心响应信号设置当前端口
    for (int i = 0; i < m_inputModel->rowCount(); ++i) {
        auto item = m_inputModel->item(i);
        auto port = item->data(Qt::WhatsThisPropertyRole).value<const Port *>();
        if (port && port->id() == portId && cardId == port->cardId()) {
            m_inputModel->removeRow(i);
            break;
        }
    }
    if (m_currentPort && m_currentPort->id() == portId && m_currentPort->cardId() == cardId) {
        m_currentPort = nullptr;
    }

    changeComboxStatus();
    showDevice();
    m_inputSoundCbx->blockSignals(false);
}

void MicrophonePage::changeComboxIndex(const int idx)
{
    if (idx < 0)
        return;

    auto tFunc = [this](const int tmpIdx) {
        auto temp = m_inputModel->index(tmpIdx, 0);
        const Port *port = m_inputModel->data(temp, Qt::WhatsThisPropertyRole).value<const Port *>();
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
    m_waitStatusChangeTimer->stop();
    m_waitStatusChangeTimer->start(m_waitTimerValue);
}

void MicrophonePage::refreshActivePortShow(const Port *port)
{
    if (port && port->isActive()) {
        m_inputSoundCbx->comboBox()->setCurrentText(port->name() + "(" + port->cardName() + ")");
        m_currentBluetoothPortStatus = port->isBluetoothPort();
        showDevice();
    }
}

void MicrophonePage::addPort(const Port *port)
{
    if (port->In == port->direction()) {
        DStandardItem *pi = new DStandardItem;
        pi->setText(port->name() + "(" + port->cardName() + ")");
        pi->setData(QVariant::fromValue<const Port *>(port), Qt::WhatsThisPropertyRole);

        connect(port, &Port::nameChanged, this, [ = ](const QString str) {
            pi->setText(str);
        });
        connect(port, &Port::isInputActiveChanged, this, [ = ](bool isActive) {
            // 若关闭设备 此时pi为空
            if (pi) {
                pi->setCheckState(isActive ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
                if (isActive) {
                    m_currentPort = port;
                    changeComboxStatus();
                }
            }
        });
        connect(port, &Port::currentPortEnabled, this, [ = ](bool isEnable) {
            int index = m_inputSoundCbx->comboBox()->findData(QVariant::fromValue(port));
            // 若端口可用 且没有添加
            if (isEnable && (index == -1) && pi) {
                m_inputModel->appendRow(pi);
                showDevice();
            }

            if (!isEnable && (index != -1)) {
                m_inputModel->removeRow(index);
                showDevice();
            }
        });

        m_inputSoundCbx->comboBox()->hidePopup();
        if (port->isEnabled())
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
    m_layout->addWidget(m_inputSlider);

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
    auto icon_high = DStyle::standardIcon(style(), DStyle::SP_MediaVolumeHighElement);
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
    slider2->setLeftIcon(DIconTheme::findQIcon("dcc_feedbacklow"));
    slider2->setRightIcon(DIconTheme::findQIcon("dcc_feedbackhigh"));
    slider2->setIconSize(QSize(24, 24));
    slider2->setTickInterval(1);
    slider2->setPageStep(1);

    connect(m_model, &SoundModel::isPortEnableChanged, m_noiseReductionsw, &ComboxWidget::setVisible);
    m_conn = connect(m_model, &SoundModel::microphoneFeedbackChanged, [ = ](double vol2) {
        // TODO: gsettings 待改为dconfig
//        int pos = GSettingWatcher::instance()->getStatus("soundFeedbackSlider") == "Disabled" ? 0 : (int(vol2 * 100));
        int pos = int(vol2 * 100);
        slider2->setSliderPosition(pos);
    });
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &MicrophonePage::refreshIcon);
    connect(qApp, &DApplication::iconThemeChanged, this, &MicrophonePage::refreshIcon);
    m_layout->addWidget(m_feedbackSlider);

    refreshIcon();
    showDevice();
#endif
}

#ifndef DCC_DISABLE_FEEDBACK
void MicrophonePage::initCombox()
{
    SettingsGroup *inputSoundsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    inputSoundsGrp->getLayout()->setContentsMargins(0, 0, 0, 10);
    inputSoundsGrp->appendItem(m_inputSoundCbx);

    m_layout->addWidget(m_noiseReductionsw);
    m_layout->addWidget(inputSoundsGrp);
    m_layout->setSpacing(10);
    m_layout->addStretch(10);
}
#else
void MicrophonePage::initCombox()
{

    m_noiseReductionsw->setVisible(false);
    m_inputSoundCbx->setVisible(false);
    //放到宏外面修复sw架构下音频布局异常的问题
    m_layout->addStretch(10);
}
#endif


void MicrophonePage::refreshIcon()
{
    m_volumeBtn->setIcon(DStyle::standardIcon(style(),m_model->microphoneOn() ? DStyle::SP_MediaVolumeMutedElement : DStyle::SP_MediaVolumeLowElement));
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

    setDeviceVisible(1 <= m_inputModel->rowCount());
}

void MicrophonePage::setDeviceVisible(bool visable)
{
    if (visable) {
        m_feedbackSlider->show();
        m_inputSlider->show();
        m_noiseReductionsw->setVisible(!m_currentBluetoothPortStatus);
    } else {
        m_feedbackSlider->hide();
        m_inputSlider->hide();
        m_noiseReductionsw->hide();
    }
}
