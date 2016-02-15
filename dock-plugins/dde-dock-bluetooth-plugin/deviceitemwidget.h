/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DEVICEITEMWIDGET_H
#define DEVICEITEMWIDGET_H

#include <QWidget>

#include "bluetoothobject.h"

#include <dimagebutton.h>
#include <dloadingindicator.h>

DWIDGET_USE_NAMESPACE

class DeviceItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceItemWidget(BluetoothObject::DeviceInfo *info, QWidget *parent = 0);
    ~DeviceItemWidget();

public slots:
    void updateUI();

protected:
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

private:
    void disConnect();

private:
    BluetoothObject::DeviceInfo *m_info = nullptr;

    QLabel *m_deviceName;
    DImageButton *m_removeBtn;
    DLoadingIndicator *m_loadingIndicator;
};

#endif // DEVICEITEMWIDGET_H
