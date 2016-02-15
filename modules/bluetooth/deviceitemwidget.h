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

#include "bluetoothmainwidget.h"
#include "confrimwidget.h"

#include <dimagebutton.h>
#include <dloadingindicator.h>

DWIDGET_USE_NAMESPACE

class DeviceItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceItemWidget(BluetoothMainWidget::DeviceInfo *info, QWidget *parent = 0);
    ~DeviceItemWidget();

public slots:
    void updateUI();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    void showConfirm();

private:
    BluetoothMainWidget::DeviceInfo *m_info = nullptr;

    bool m_confirmVisible = false;
    ConfrimWidget *m_confirmWidget;
    QLabel *m_deviceName;
    QLabel *m_statTips;
    DImageButton *m_removeBtn;
    DLoadingIndicator *m_loadingIndicator;
};

#endif // DEVICEITEMWIDGET_H
