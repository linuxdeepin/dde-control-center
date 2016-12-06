/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include <QWidget>

#include "adapter.h"

#include "switchwidget.h"
#include "settingsgroup.h"

namespace dcc {
namespace bluetooth {

class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(const Adapter *adapter);

    void setAdapter(const Adapter *adapter);

public slots:
    void toggleSwitch(const bool &checked);

signals:
    void requestToggleAdapter(const bool &powered);

private slots:
    void addDevice(const Device *device);
    void removeDevice(const QString &deviceId);

private:
    SwitchWidget *m_switch;
    SettingsGroup *m_titleGroup;
    SettingsGroup *m_myDevicesGroup;
    SettingsGroup *m_otherDevicesGroup;
};

}
}

#endif // ADAPTERWIDGET_H
