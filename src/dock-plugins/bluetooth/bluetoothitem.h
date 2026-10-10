// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BLUETOOTHITEM_H
#define BLUETOOTHITEM_H

#include "componments/device.h"
#include "componments/adaptersmanager.h"
#include "commoniconbutton.h"

#include <QWidget>

#define BLUETOOTH_KEY "bluetooth-item-key"

class BluetoothApplet;
class QuickPanelWidget;

namespace Dock {
class TipsWidget;
}
class BluetoothItem : public QWidget
{
    Q_OBJECT

public:
    explicit BluetoothItem(AdaptersManager *adapterManager, QWidget *parent = nullptr);

    QWidget *tipsWidget();
    BluetoothApplet *popupApplet();
    QWidget *quickPanel();
    QWidget *dockItemWidget() const { return m_iconWidget; };

    const QString contextMenu() const;
    void invokeMenuItem(const QString menuId, const bool checked);

    void refreshIcon();
    void refreshTips();

    bool hasAdapter();

protected:
    void resizeEvent(QResizeEvent *event);

signals:
    void noAdapter();
    void justHasAdapter();
    void requestExpand();
    void requestHideApplet();

private:
    Dock::TipsWidget *m_tipsLabel;
    BluetoothApplet *m_applet;
    QuickPanelWidget *m_quickPanel;
    CommonIconButton *m_iconWidget;
    Device::State m_devState;
    bool m_adapterPowered;
};

#endif // BLUETOOTHITEM_H
