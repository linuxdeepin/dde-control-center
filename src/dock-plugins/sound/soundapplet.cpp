// SPDX-FileCopyrightText: 2011 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "soundapplet.h"
#include "utils.h"
#include "soundmodel.h"
#include "dock-constants.h"
#include "constants.h"

#include <DApplication>
#include <DFontSizeManager>
#include <DGuiApplicationHelper>
#include <DStandardItem>

#include <QIcon>
#include <QLabel>
#include <QListIterator>
#include <QPainter>
#include <QScrollBar>

#define SOUND_OUTPUT_SLIDER "soundOutputSlider"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
using namespace Dock;

const static int ITEM_SPACING = 10;

SoundApplet::SoundApplet(QWidget* parent)
    : QWidget(parent)
    , m_volumeSlider(new DockSlider(this))
    , m_volumeLabel(new QLabel(this))
    , m_deviceLabel(new QLabel(this))
    , m_outputLabel(new QLabel(this))
    , m_deviceWidget(new QWidget(this))
    , m_sliderContainer(new SliderContainer(this))
    , m_audioInter(SoundController::ref().audioInter())
    , m_defSinkInter(nullptr)
    , m_listView(new PluginListView(this))
    , m_settingButton(new JumpSettingButton(this))
    , m_itemModel(new QStandardItemModel(m_listView))
    , m_dConfig(Dtk::Core::DConfig::create("org.deepin.dde.tray-loader", "org.deepin.dde.dock.plugin.sound", QString(), this))
    , m_minHeight(-1)
    , m_spacerItem(new QSpacerItem(Dock::DOCK_POPUP_WIDGET_WIDTH, ITEM_SPACING))
{
    initUi();
    initConnections();
}

void SoundApplet::initUi()
{
    setFixedWidth(Dock::DOCK_POPUP_WIDGET_WIDTH);
    setMaximumHeight(Dock::DOCK_POPUP_WIDGET_MAX_HEIGHT);
    // Title
    m_deviceLabel->setText(tr("Volume"));
    DFontSizeManager::instance()->bind(m_deviceLabel, DFontSizeManager::T9, QFont::Medium);
    m_volumeLabel->setText(QString("%1%").arg(0));
    DFontSizeManager::instance()->bind(m_volumeLabel, DFontSizeManager::T9, QFont::Medium);
    QHBoxLayout* deviceLayout = new QHBoxLayout(m_deviceWidget);
    deviceLayout->setSpacing(0);
    deviceLayout->setContentsMargins(0, 0, 0, 0);
    deviceLayout->setContentsMargins(10, 0, 10, 0);
    deviceLayout->addWidget(m_deviceLabel, 0, Qt::AlignLeft);
    deviceLayout->addWidget(m_volumeLabel, 0, Qt::AlignRight);

    // Volume slider
    m_volumeSlider->setFixedHeight(36);
    m_volumeSlider->setMinimum(0);
    m_volumeSlider->setMaximum(SoundModel::ref().maxVolumeUI());
    m_volumeSlider->setPageStep(2);
    m_sliderContainer->setSlider(m_volumeSlider);
    m_sliderContainer->addBackground();
    m_sliderContainer->setButtonsSize(QSize(16, 16));
    m_sliderContainer->setFixedHeight(36);
    updateVolumeSliderStatus(m_dConfig->value(SOUND_OUTPUT_SLIDER, Enabled).toInt());
    refreshIcon();

    // Output
    m_outputLabel->setText(tr("Output"));
    m_outputLabel->setContentsMargins(10, 0, 0, 0);
    DFontSizeManager::instance()->bind(m_outputLabel, DFontSizeManager::T9, QFont::Medium);
    // Port list
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setModel(m_itemModel);
    m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listView->installEventFilter(this);

    // sound setting button
    m_settingButton->setAutoShowPage(true);
    m_settingButton->setDccPage("sound", "output");
    m_settingButton->setIcon(QIcon::fromTheme("open-arrow"));
    m_settingButton->setDescription(tr("Sound settings"));

    m_centralLayout = new QVBoxLayout(this);
    m_centralLayout->setContentsMargins(10,10, 10, 10);
    m_centralLayout->setSpacing(0);
    m_centralLayout->addWidget(m_deviceWidget);
    m_centralLayout->addSpacing(5);
    m_centralLayout->addWidget(m_sliderContainer);
    m_centralLayout->addSpacing(ITEM_SPACING);
    m_centralLayout->addWidget(m_outputLabel);
    m_centralLayout->addSpacing(5);
    m_centralLayout->addWidget(m_listView);
    m_centralLayout->addSpacerItem(m_spacerItem);
    m_centralLayout->addWidget(m_settingButton);

    m_volumeSlider->setAccessibleName("volume_slider");
    m_volumeLabel->setAccessibleName("volume_sound_tips");

    updatePorts();
}

void SoundApplet::initConnections()
{
    if (m_dConfig) {
        connect(m_dConfig, &Dtk::Core::DConfig::valueChanged, this, [ = ] (const QString &key) {
            if (key == SOUND_OUTPUT_SLIDER) {
                updateVolumeSliderStatus(m_dConfig->value(SOUND_OUTPUT_SLIDER).toInt());
            }
        });
    }
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &SoundApplet::refreshIcon);
    connect(qApp, &DApplication::iconThemeChanged, this, &SoundApplet::refreshIcon);

    connect(m_volumeSlider, &DockSlider::valueChanged, this, &SoundApplet::volumeSliderValueChanged);
    connect(m_sliderContainer, &SliderContainer::iconClicked, this, [this](SliderContainer::IconPosition icon) {
        if (icon == SliderContainer::LeftIcon && SoundController::ref().existActiveOutputDevice() && m_defSinkInter) {
            m_defSinkInter->SetMuteQueued(!m_defSinkInter->mute());
        }
    });
    connect(m_listView, &PluginListView::clicked, this, [this](const QModelIndex& idx) {
        auto key = m_listView->model()->data(idx, Qt::WhatsThisPropertyRole).value<QString>();
        const SoundCardPort* port = SoundModel::ref().port(key);
        if (port) {
            m_audioInter->SetPort(port->cardId(), port->portName(), int(port->direction()));
            // 手动勾选时启用设备
            m_audioInter->SetPortEnabled(port->cardId(), port->portName(), true);
        }
    });
    connect(&SoundController::ref(), &SoundController::defaultSinkChanged, this, &SoundApplet::updatePorts);

    connect(&SoundModel::ref(), &SoundModel::maxVolumeUIChanged, this, &SoundApplet::maxUiVolumeChanged);
    connect(&SoundModel::ref(), &SoundModel::enabledPortAdded, this, &SoundApplet::addPort);
    connect(&SoundModel::ref(), &SoundModel::cardsInfoChanged, this, &SoundApplet::updatePorts);
    connect(&SoundModel::ref(), &SoundModel::portRemoved, this, &SoundApplet::removePort);
    connect(&SoundModel::ref(), &SoundModel::volumeChanged, this, [this] (int volume) {
        onVolumeChanged(volume);
    });
    connect(&SoundModel::ref(), &SoundModel::muteStateChanged, this, [this] {
        onVolumeChanged(SoundController::ref().existActiveOutputDevice() ? SoundModel::ref().volume() : 0);
    });
    connect(m_settingButton, &JumpSettingButton::showPageRequestWasSended, this, &SoundApplet::requestHideApplet);

    // 初始化数据
    updatePorts();
    for (const auto port : SoundModel::ref().ports()) {
        addPort(port);
    }
}

DockSlider* SoundApplet::mainSlider()
{
    return m_volumeSlider;
}

void SoundApplet::onDefaultSinkChanged()
{
    m_defSinkInter = SoundController::ref().defaultSinkInter();

    // 无声卡状态下，会有伪sink设备，显示音量为0
    // 支持云平台无声卡显示
    onVolumeChanged(SoundController::ref().existActiveOutputDevice() ? SoundModel::ref().volume() : 0);
}

void SoundApplet::onVolumeChanged(int volume)
{
    m_volumeSlider->setValue(std::min(150, volume));
    m_volumeLabel->setText(QString::number(volume) + '%');
    refreshIcon();
}

void SoundApplet::volumeSliderValueChanged()
{
    SoundController::ref().SetVolume(m_volumeSlider->value() * 0.01, true);
}

void SoundApplet::updatePorts()
{
    // 重新获取切换的设备信息
    onDefaultSinkChanged();

    // 判断是否存在激活的输出设备
    enableDevice(SoundController::ref().existActiveOutputDevice());

    // 一个设备不展示列表，直接可以调整声音
    const bool visible = m_itemModel->rowCount() > 0;
    m_listView->setVisible(visible);
    m_outputLabel->setVisible(visible);

    m_spacerItem->changeSize(Dock::DOCK_POPUP_WIDGET_WIDTH, ITEM_SPACING,
        QSizePolicy::Minimum,  visible ? QSizePolicy::Minimum : QSizePolicy::Expanding);

    resizeApplet();
}

void SoundApplet::maxUiVolumeChanged()
{
    m_volumeSlider->setMaximum(SoundModel::ref().maxVolumeUI());
    m_volumeSlider->setValue(std::min(150, SoundModel::ref().volume()));
}

void SoundApplet::refreshIcon()
{
    const bool mute = SoundController::ref().existActiveOutputDevice() ? SoundModel::ref().isMute() : true;
    QString volumeString = mute ? "muted" : "off";

    m_sliderContainer->setIcon(SliderContainer::LeftIcon, QString("audio-volume-%1-symbolic").arg(volumeString), 0);
    m_sliderContainer->setIcon(SliderContainer::RightIcon, "audio-volume-high-symbolic", 0);
}

PluginStandardItem* SoundApplet::findItem(const QString &uniqueKey) const
{
    for (int i = 0; i < m_itemModel->rowCount(); i++) {
        auto item = m_itemModel->item(i);
        if (uniqueKey == item->data(Qt::WhatsThisPropertyRole).value<QString>()) {
            return dynamic_cast<PluginStandardItem*>(item);
        }
    }

    return nullptr;
}

void SoundApplet::selectItem(PluginStandardItem *targetItem)
{
    if (!targetItem) {
        return;
    }

    for (int i = 0; i < m_itemModel->rowCount(); ++i) {
        auto item = dynamic_cast<PluginStandardItem *>(m_itemModel->item(i, 0));
        if (!item) {
            continue;
        }
        item->updateState(item == targetItem ? PluginItemState::ConnectedOnlyPrompt : PluginItemState::NoState);
    }
}

void SoundApplet::addPort(const SoundCardPort* port)
{
    if (!port->isEnabled())
        return;

    PluginStandardItem* pi = new PluginStandardItem(QIcon::fromTheme(SoundCardPort::icon(port->portType())), 
                                    port->description() + "(" + port->cardName() + ")");
    pi->setData(port->uniqueKey(), Qt::WhatsThisPropertyRole);

    connect(port, &SoundCardPort::nameChanged, this, [this, port](const QString& str) {
        QString devName = str + "(" + port->cardName() + ")";
        auto item = findItem(port->uniqueKey());
        if (item)
            item->updateName(devName);
    });
    connect(port, &SoundCardPort::cardNameChanged, this, [this, port](const QString& str) {
        QString devName = port->description() + "(" + str + ")";
        auto item = findItem(port->uniqueKey());
        if (item)
            item->updateName(devName);
    });
    connect(port, &SoundCardPort::activityChanged, this, [this, port](bool isActive) {
        auto item = findItem(port->uniqueKey());
        if (item && isActive)
            selectItem(item);
    });
    connect(port, &SoundCardPort::enabledChanged, this, [port, this] (bool enabled) {
        if (!enabled) {
            removeDisabledDevice(port->portName(), port->cardId());
        }
    });
    m_itemModel->appendRow(pi);
    m_itemModel->sort(0);
    if (port->isActive()) {
        selectItem(pi);
    }

    updatePorts();
}

void SoundApplet::removePort(const QString& key)
{
    for (int i = 0; i < m_itemModel->rowCount();) {
        auto item = m_itemModel->item(i);
        if (key == item->data(Qt::WhatsThisPropertyRole).value<QString>()) {
            m_itemModel->removeRow(i);
            break;
        } else {
            ++i;
        }
    }
    auto port = SoundModel::ref().port(key);
    if (port)
        port->disconnect(this);
    updatePorts();
}

void SoundApplet::enableDevice(bool flag)
{
    int status = m_dConfig ? m_dConfig->value(SOUND_OUTPUT_SLIDER).toInt() : Enabled;
    m_volumeSlider->setEnabled(Disabled == status ? false : flag);
    m_sliderContainer->setButtonsEnabled(flag);
}

/**
 * @brief SoundApplet::removeDisabledDevice 移除禁用设备
 * @param portId
 * @param cardId
 */
void SoundApplet::removeDisabledDevice(QString portName, unsigned int cardId)
{
    removePort(SoundCardPort::compositeKey(cardId, portName));
    if (m_defSinkInter && m_defSinkInter->activePort().name == portName && m_defSinkInter->card() == cardId) {
        enableDevice(false);
    }
}

void SoundApplet::updateVolumeSliderStatus(int status)
{
    const bool enabled = (Enabled == status);
    m_volumeSlider->setEnabled(enabled);
    m_sliderContainer->setButtonsEnabled(enabled);

    const bool visible = (Hidden != status);
    m_sliderContainer->setVisible(visible);
}

void SoundApplet::resizeApplet()
{
    const int listViewHeight = m_itemModel->rowCount() * (m_listView->getItemHeight()+m_listView->getItemSpacing()) - m_listView->getItemSpacing();
    int totalHeight = m_deviceWidget->height() + m_sliderContainer->height() + m_settingButton->height() + ITEM_SPACING * 4;
    totalHeight += m_outputLabel->height() + (m_outputLabel->isVisible() ? ITEM_SPACING : 0);
    totalHeight += listViewHeight + (listViewHeight > 0 ? ITEM_SPACING : 0);

    resize(width(), qMin(Dock::DOCK_POPUP_WIDGET_MAX_HEIGHT, qMax(m_minHeight, totalHeight)));
}

bool SoundApplet::eventFilter(QObject *watcher, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        resizeApplet();
    }

    return QWidget::eventFilter(watcher, event);
}

void SoundApplet::setMinHeight(int minHeight)
{
    m_minHeight = minHeight;
    resizeApplet();
}
