/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include <QWidget>

#include <libdui/dlistwidget.h>
#include <libdui/dloadingindicator.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dlabel.h>

#include "bluetoothobject.h"

DUI_USE_NAMESPACE

class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(BluetoothObject::AdapterInfo *info,
                           QWidget *parent = 0);
    ~AdapterWidget();

    void addDevice(BluetoothObject::DeviceInfo *info);
    void removeDevice(BluetoothObject::DeviceInfo *info, bool isDelete);
    int getHeightHint() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;

signals:
    void sizeChanged();

public slots:
    void updateUI();

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private:
    void initUI();

    DListWidget *m_deviceItemList = nullptr;
    DSeparatorHorizontal *m_listWidgetSeparator = nullptr;
    BluetoothObject::AdapterInfo *m_info = nullptr;
    DHeaderLine *m_headerLine = nullptr;
    DSwitchButton *m_bluetoothSwitch;
};

#endif // ADAPTERWIDGET_H
