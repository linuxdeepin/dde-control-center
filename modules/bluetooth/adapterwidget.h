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

#include "contentwidget.h"
#include "adapter.h"
#include "TitleEdit.h"
#include "switchwidget.h"
#include "settingsgroup.h"

using namespace dcc::widgets;

namespace dcc {
namespace bluetooth {

class AdapterWidget : public ContentWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(const Adapter *adapter);

    void setAdapter(const Adapter *adapter);
    const Adapter *adapter() const;

public slots:
    void toggleSwitch(const bool &checked);

signals:
    void requestSetToggleAdapter(const Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const Device *device);
    void requestShowDetail(const Adapter *adapter, const Device *device);
    void requestSetAlias(const Adapter *adapter, const QString &alias);

private slots:
    void addDevice(const Device *device);
    void removeDevice(const QString &deviceId);

private:
    const Adapter *m_adapter;
    TitleEdit *m_titleEdit;
    SwitchWidget *m_switch;
    SettingsGroup *m_titleGroup;
    SettingsGroup *m_myDevicesGroup;
    SettingsGroup *m_otherDevicesGroup;
    QLabel        *m_tip;
};

}
}

#endif // ADAPTERWIDGET_H
