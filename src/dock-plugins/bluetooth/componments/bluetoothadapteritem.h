// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BLUETOOTHADAPTERITEM_H
#define BLUETOOTHADAPTERITEM_H

#include "commoniconbutton.h"
#include "device.h"
#include "bluetoothapplet.h"
#include "pluginlistview.h"
#include "bluetooth1interface.h"

#include <DListView>
#include <DStyleHelper>

#include <QPushButton>

using  DBusBluetooth = org::deepin::dde::Bluetooth1;

DWIDGET_USE_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DSwitchButton;
DWIDGET_END_NAMESPACE

class Adapter;
class SettingLabel;
class QStandardItemModel;
class RefreshButton;
class HorizontalSeparator;
class StateButton;
class ListViewDelegate;

class BluetoothDeviceItem : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothDeviceItem(QStyle *style = nullptr, const Device *device = nullptr, PluginListView *parent = nullptr);
    virtual ~BluetoothDeviceItem();

    PluginStandardItem *standardItem() { return m_standardItem; }
    const Device *device() { return m_device; }

public slots:
    // 更新蓝牙设备的连接状态
    void updateDeviceState(Device::State state);

signals:
    void requestTopDeviceItem(PluginStandardItem *item);
    void deviceStateChanged(const Device *device);
    void disconnectDevice();

private:
    void initConnect();

    DStyleHelper m_style;

    const Device *m_device;
    PluginStandardItem *m_standardItem;
};

class DeviceControlWidget : public QPushButton
{
    Q_OBJECT
public:
    DeviceControlWidget(QWidget *parent = nullptr)
        : QPushButton(parent)
        , m_text(tr("Other Devices"))
        , m_icon(QIcon::fromTheme("arrow-up"))
        , m_isExpand(false)
    {
        setFixedHeight(24);
        connect(this, &QPushButton::clicked, this, [this] {
            setExpandState(!m_isExpand);
        });
    }

    void setIcon(const QIcon &icon) {
        m_icon = icon;
        update();
    }
    void setExpandState(bool isExpand) {
        if (m_isExpand != isExpand) {
            m_isExpand = isExpand;
            Q_EMIT expandStateChanged(m_isExpand);
            update();
        }
    }

    bool isExpand() { return m_isExpand; }

Q_SIGNALS:
    void expandStateChanged(bool state);


protected:
    void paintEvent(QPaintEvent *paint) override
    {
        Q_UNUSED(paint)
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        // 绘制背景
        painter.setPen(Qt::NoPen);
        QColor bgNomal = palette().brightText().color();
        bgNomal.setAlphaF(0.05);
        QColor background = underMouse() ? palette().color(QPalette::Highlight) : bgNomal;
        painter.setBrush(background);
        painter.drawRoundedRect(rect(), 8, 8);
        // 绘制文字
        QRect textRect = rect().marginsRemoved(QMargins(10, 0, 0, 0));
        QColor textNomal = palette().brightText().color();
        textNomal.setAlphaF(0.6);
        QColor textColor = underMouse() ? palette().color(QPalette::HighlightedText) : textNomal;
        painter.setPen(textColor);
        auto pFont = painter.font();
        pFont.setWeight(QFont::Normal);
        pFont.setPixelSize(DFontSizeManager::instance()->fontPixelSize(DFontSizeManager::T10));
        painter.setFont(pFont);
        QTextOption opt;
        opt.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        painter.drawText(textRect, m_text, opt);
        // 绘制图标
        painter.setPen(textColor);
        QRect iconRect = QRect(rect().x() + rect().width() - 22, rect().y() + 6, 12, 12);
        if (!m_isExpand) {
            painter.translate(iconRect.x() + iconRect.width() / 2, rect().height() / 2);
            painter.rotate(180);
            painter.translate(-(iconRect.x() + iconRect.width() / 2), -(rect().height() / 2));
        }
        m_icon.paint(&painter, iconRect);
    }

private:
    QString m_text;
    QIcon m_icon;
    bool m_isExpand;
};

class BluetoothAdapterItem : public QWidget
{
    Q_OBJECT
public:
    explicit BluetoothAdapterItem(Adapter *adapter, QWidget *parent = nullptr);
    ~BluetoothAdapterItem();
    Adapter *adapter() { return m_adapter; }
    QStringList connectedDevicesName();
    void setStateBtnEnabled(bool);

public slots:
    // 添加蓝牙设备
    void onDeviceAdded(const Device *device);
    // 移除蓝牙设备
    void onDeviceRemoved(const Device *device);
    // 蓝牙设备名称更新
    void onDeviceNameUpdated(const Device *device);
    // 连接蓝牙设备
    void onConnectDevice(const QModelIndex &index);
    // 将已连接的蓝牙设备放到列表第一个
    void onTopDeviceItem(PluginStandardItem *item);
    // 设置蓝牙适配器名称
    void onAdapterNameChanged(const QString name);

    QSize sizeHint() const override;

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void adapterPowerChanged();
    void requestSetAdapterPower(Adapter *adapter, bool state);
    void requestRefreshAdapter(Adapter *adapter);
    void connectDevice(const Device *device, Adapter *adapter);
    void deviceCountChanged();
    void deviceStateChanged(const Device *device);

private:
    void initData();
    void initUi();
    void initConnect();
    void setUnnamedDevicesVisible(bool isShow);
    void updateMyDeviceLabelTheme();
    void clearDeviceList();

    Adapter *m_adapter;
    SettingLabel *m_adapterLabel;
    QVBoxLayout *m_adapterLayout;
    DSwitchButton *m_adapterStateBtn;
    QWidget *m_myDeviceWidget;
    DLabel *m_myDeviceLabel;
    PluginListView *m_myDeviceListView;
    QStandardItemModel *m_myDeviceModel;

    DeviceControlWidget *m_otherDeviceControlWidget;
    PluginListView *m_otherDeviceListView;
    QStandardItemModel *m_otherDeviceModel;
    CommonIconButton *m_refreshBtn;
    DBusBluetooth *m_bluetoothInter;
    bool m_showUnnamedDevices;
    bool m_stateBtnEnabled;
    bool m_adapterSwitchEnabled;

    QTimer *m_scanTimer;

    QMap<const Device *, BluetoothDeviceItem *> m_deviceItems;
};

#endif // BLUETOOTHADAPTERITEM_H
