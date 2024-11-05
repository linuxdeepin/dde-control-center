// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bluetoothdevicemodel.h"

#include "bluetoothadapter.h"

#include <DDesktopServices>
#include <DGuiApplicationHelper>
#include <DIconTheme>
#include <DSpinner>

#include <QApplication>
#include <QLineEdit>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QWidget>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

struct BluetoothDeviceItemAction
{
    const BluetoothDevice *device;
    DViewItemAction *spinnerAction;
    DViewItemAction *percentageText;
    DViewItemAction *percentageIcon;
    DViewItemAction *textAction;
    DViewItemAction *spaceAction;
    DViewItemAction *iconAction;
    DSpinner *loadingIndicator;
    DViewItemActionList actionList;
    DStandardItem *item;

    explicit BluetoothDeviceItemAction(const BluetoothDevice *_device)
        : device(_device)
        , spinnerAction(new DViewItemAction(Qt::AlignVCenter, QSize(), QSize(), false))
        , percentageText(new DViewItemAction(Qt::AlignVCenter, QSize(), QSize(), false))
        , percentageIcon(new DViewItemAction(Qt::AlignVCenter, QSize(), QSize(), false))
        , textAction(new DViewItemAction(Qt::AlignVCenter, QSize(), QSize(), true))
        , spaceAction(new DViewItemAction(Qt::AlignVCenter, QSize(), QSize(), false))
        , iconAction(new DViewItemAction(Qt::AlignVCenter, QSize(), QSize(), true))
        , loadingIndicator(nullptr)
        , item(new DStandardItem())
    {
        iconAction->setData(static_cast<const void *>(device));
        actionList.append(percentageText);
        actionList.append(percentageIcon);
        actionList.append(spinnerAction);
        actionList.append(textAction);
        actionList.append(spaceAction);
        actionList.append(iconAction);
        percentageText->setVisible(false);
        percentageIcon->setVisible(false);
        spinnerAction->setVisible(false);
        item->setActionList(Qt::Edge::RightEdge, actionList);
    }

    ~BluetoothDeviceItemAction()
    {
        delete item;
        if (loadingIndicator)
            delete loadingIndicator;
    }

    void setLoading(bool isLoading, QWidget *parentView)
    {
        if (spinnerAction->isVisible() == isLoading)
            return;
        if (isLoading) {
            QAbstractItemView *view = qobject_cast<QAbstractItemView *>(parentView);
            QWidget *parentWidget = view ? view->viewport() : parentView;
            if (!loadingIndicator) {
                loadingIndicator = new DSpinner(parentWidget);
                loadingIndicator->setFixedSize(24, 24);
                spinnerAction->setWidget(loadingIndicator);
                loadingIndicator->connect(loadingIndicator,
                                          &QWidget::destroyed,
                                          loadingIndicator,
                                          [this]() {
                                              loadingIndicator = nullptr;
                                          });
            }
            loadingIndicator->setParent(parentWidget);
            loadingIndicator->start();
        } else if (loadingIndicator) {
            loadingIndicator->stop();
            loadingIndicator->setVisible(false);
        }
        spinnerAction->setVisible(isLoading);
        textAction->setVisible(!isLoading);
    }
    Q_DISABLE_COPY(BluetoothDeviceItemAction)
};

const QMap<QString, QString> BluetoothDeviceModel::deviceType2Icon = {
    {"unknow", "other"},
    {"computer", "pc"},
    {"phone", "phone"},
    {"video-display", "vidicon"},
    {"multimedia-player", "tv"},
    {"scanner", "scaner"},
    {"input-keyboard", "keyboard"},
    {"input-mouse", "mouse"},
    {"input-gaming", "other"},
    {"input-tablet", "touchpad"},
    {"audio-card", "pheadset"},
    {"audio-headset", "pheadset"},
    {"network-wireless", "lan"},
    {"camera-video", "vidicon"},
    {"printer", "print"},
    {"camera-photo", "camera"},
    {"modem", "other"}
};

BluetoothDeviceModel::BluetoothDeviceModel(const BluetoothAdapter *adapter,
                                           bool paired,
                                           QWidget *parent)
    : QAbstractItemModel(parent)
    , m_paired(paired)
    , m_adapter(adapter)
    , m_parent(parent)
    , m_showAnonymous(false)
{
    for (QString id : m_adapter->devicesId()) {
        if (m_adapter->devices().contains(id)) {
            const BluetoothDevice *device = m_adapter->devices()[id];
            addDevice(device);
        }
    }
    connect(adapter,
            &BluetoothAdapter::deviceAdded,
            this,
            &BluetoothDeviceModel::addDevice,
            Qt::QueuedConnection);
    connect(adapter,
            &BluetoothAdapter::deviceRemoved,
            this,
            &BluetoothDeviceModel::removeDevice,
            Qt::QueuedConnection);
}

BluetoothDeviceModel::~BluetoothDeviceModel()
{
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        delete (*it);
    }
}

QModelIndex BluetoothDeviceModel::index(const BluetoothDevice *device)
{
    int row = 0;
    for (auto it = m_data.begin(); it != m_data.end(); ++it, ++row) {
        if ((*it)->device == device) {
            return index(row, 0);
        }
    }
    return QModelIndex();
}

// Basic functionality:
QModelIndex BluetoothDeviceModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row < 0 || row >= m_data.size())
        return QModelIndex();
    return createIndex(row, column, const_cast<BluetoothDevice *>(m_data.at(row)->device));
}

QModelIndex BluetoothDeviceModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

int BluetoothDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

int BluetoothDeviceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant BluetoothDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto themeType = DGuiApplicationHelper::instance()->themeType();
    bool isDarkTheme = themeType == DGuiApplicationHelper::DarkType;

    int row = index.row();
    const BluetoothDevice *device = m_data.at(row)->device;
    const QString iconSuffix = deviceType2Icon[device->deviceType()];

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return device->alias().isEmpty() ? device->name() : device->alias();
    case Qt::DecorationRole:
        if (!iconSuffix.isEmpty()) {
            QString deviceIconName = QString("bluetooth_%1").arg(iconSuffix);
            return QIcon::fromTheme(deviceIconName);
        }
        else
            return QIcon::fromTheme(QString("bluetooth_other"));
    case Dtk::RightActionListRole:
        return m_data.at(row)->item->data(role);
    default:
        return QVariant();
    }
}

bool BluetoothDeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const BluetoothDevice *device =
                static_cast<const BluetoothDevice *>(index.internalPointer());
        QString devAlias = value.toString();
        QString devName(device->name());
        if (devAlias.isEmpty()) {
            Q_EMIT requestSetDevAlias(device, devName);
        } else {
            Q_EMIT requestSetDevAlias(device, devAlias);
        }
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}

Qt::ItemFlags BluetoothDeviceModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);
    int row = index.row();
    const BluetoothDevice *device = m_data.at(row)->device;
    // INFO: when is headset, not connect twice
    if (device
        && (device->deviceType() == "audio-headset" || device->deviceType() == "autio-headphones")
        && device->state() == BluetoothDevice::StateAvailable)
        flag.setFlag(Qt::ItemIsEnabled, false);
    return flag | Qt::ItemIsEditable;
}

void BluetoothDeviceModel::addDevice(const BluetoothDevice *device)
{
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        if ((*it)->device == device) {
            return;
        }
    }

    // clang-format off
    connect(device, &BluetoothDevice::pairedChanged, this, &BluetoothDeviceModel::onPairedChanged, Qt::UniqueConnection);
    if (device->paired() != m_paired)
        return;
    connect(device, &BluetoothDevice::nameChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::aliasChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::stateChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::trustedChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::connectingChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::batteryChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);

    BluetoothDeviceItemAction *item = new BluetoothDeviceItemAction(device);
    updateItem(item);
    connect(item->iconAction, &DViewItemAction::triggered, this, [this]() {
        DViewItemAction *action = qobject_cast<DViewItemAction *>(sender());
        for (auto it = m_data.begin(); it != m_data.end(); ++it) {
            if ((*it)->iconAction == action) {
                emit detailClick((*it)->device);
                break;
            }
        }
    });
    // clang-format on
    m_allData.append(item);
    if (m_showAnonymous || !device->name().isEmpty()) {
        beginInsertRows(QModelIndex(), 0, 0);
        m_data.prepend(item);
        endInsertRows();
    }
}

void BluetoothDeviceModel::onPairedChanged(const bool &paired)
{
    const BluetoothDevice *device = qobject_cast<const BluetoothDevice *>(sender());
    if (!device)
        return;
    if (paired == m_paired) {
        addDevice(device);
    } else {
        removeDevice(device->id());
    }
}

void BluetoothDeviceModel::updateData()
{
    const BluetoothDevice *device = qobject_cast<const BluetoothDevice *>(sender());
    int row = 0;
    for (auto it = m_data.begin(); it != m_data.end(); ++it, ++row) {
        if ((*it)->device == device) {
            updateItem(*it);
            emit dataChanged(index(row, 0), index(row, 0));
            break;
        }
    }
}

void BluetoothDeviceModel::removeDevice(const QString &deviceId)
{
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        if ((*it)->device->id() == deviceId) {
            BluetoothDeviceItemAction *item = *it;
            m_allData.removeOne(item);
            int row = m_data.indexOf(item);
            if (row != -1) {
                beginRemoveRows(QModelIndex(), row, row);
                m_data.removeOne(item);
                endRemoveRows();
            }
            delete item;
            break;
        }
    }
}

void BluetoothDeviceModel::updateItem(BluetoothDeviceItemAction *item)
{
    const BluetoothDevice *device = item->device;
    if (device->state() == BluetoothDevice::StateAvailable) {
        item->setLoading(true, m_parent);
    } else {
        switch (device->state()) {
        case BluetoothDevice::StateConnected:
            if (device->connectState()) {
                item->textAction->setText(tr("Connected"));
                item->setLoading(false, m_parent);
            }
            break;
        case BluetoothDevice::StateUnavailable:
        case BluetoothDevice::StateDisconnecting: {
            item->textAction->setText(tr("Not connected"));
            item->setLoading(false, m_parent);
        } break;
        default:
            break;
        }
    }
    item->iconAction->setVisible(m_paired);
    if (m_paired) {
        item->iconAction->setIcon(m_parent->style()->standardIcon(QStyle::SP_ArrowRight));
        int battery = device->battery();
        if (battery != 0) {
            item->percentageIcon->setVisible(true);
            item->percentageIcon->setIcon(getBatteryIcon(battery));
            item->percentageText->setVisible(true);
            item->percentageText->setText(QString("%1%").arg(battery));
            return;
        }
    }
    item->percentageIcon->setVisible(false);
    item->percentageText->setVisible(false);
}

QIcon BluetoothDeviceModel::getBatteryIcon(int percentage)
{
    /* 0-5%、6-10%、11%-20%、21-30%、31-40%、41-50%、51-60%、61%-70%、71-80%、81-90%、91-100% */
    QString percentageStr;
    if (percentage <= 5) {
        percentageStr = "000";
    } else if (percentage <= 10) {
        percentageStr = "010";
    } else if (percentage <= 20) {
        percentageStr = "020";
    } else if (percentage <= 30) {
        percentageStr = "030";
    } else if (percentage <= 40) {
        percentageStr = "040";
    } else if (percentage <= 50) {
        percentageStr = "050";
    } else if (percentage <= 60) {
        percentageStr = "060";
    } else if (percentage <= 70) {
        percentageStr = "070";
    } else if (percentage <= 80) {
        percentageStr = "080";
    } else if (percentage <= 90) {
        percentageStr = "090";
    } else if (percentage <= 100) {
        percentageStr = "100";
    } else {
        percentageStr = "unknow";
    }

    QString iconName = QString("battery-%1-symbolic").arg(percentageStr);
    QIcon qrcIcon = DIconTheme::findQIcon(iconName, DIconTheme::DontFallbackToQIconFromTheme);
    auto themeType = DGuiApplicationHelper::instance()->themeType();
    bool isDarkTheme = themeType == DGuiApplicationHelper::DarkType;
    // "-dark" means dark icon, not icon for dark theme.
    QString iconNameSystem = isDarkTheme ? iconName : iconName + "-dark";
    return DIconTheme::findQIcon(iconNameSystem, qrcIcon, DIconTheme::IgnoreBuiltinIcons);
}

void BluetoothDeviceModel::showAnonymous(bool show)
{
    if (m_showAnonymous == show)
        return;
    m_showAnonymous = show;
    beginResetModel();
    m_data.clear();
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        if (m_showAnonymous || !(*it)->device->name().isEmpty())
            m_data.append(*it);
    }
    endResetModel();
}

BluetoothDeviceDelegate::BluetoothDeviceDelegate(QAbstractItemView *parent)
    : Dtk::Widget::DStyledItemDelegate(parent)
{
}

QWidget *BluetoothDeviceDelegate::createEditor(QWidget *parent,
                                               const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    Q_UNUSED(option)
    if (!index.isValid())
        return nullptr;

    QLineEdit *edit = new QLineEdit(parent);
    edit->setFrame(false);
    edit->setSizePolicy(QSizePolicy::Ignored, edit->sizePolicy().verticalPolicy());
    connect(edit, &QLineEdit::textChanged, edit, [edit](const QString &str) {
        if (str.length() > 32) {
            edit->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    });
    return edit;
}
