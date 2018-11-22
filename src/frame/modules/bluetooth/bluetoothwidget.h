/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DCC_BLUETOOTH_BLUETOOTHWIDGET_H
#define DCC_BLUETOOTH_BLUETOOTHWIDGET_H

#include "modules/modulewidget.h"
#include "widgets/nextpagewidget.h"
#include "bluetoothmodel.h"
#include "adapterwidget.h"

namespace dcc {
namespace bluetooth {

class BluetoothWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit BluetoothWidget(BluetoothModel *model);
    void setModel(BluetoothModel *model);
//    AdapterWidget *widgetByAdapterId(const QString &adapterId);

Q_SIGNALS:
    void showBluetoothDetail(const Adapter *adapter) const;
    void requestModuleVisible(const bool visible) const;

public Q_SLOTS:
    void addAdapter(const Adapter *adapter);
    void removeAdapter(const Adapter *adapter);

private:
    void setVisibleState();

private:
    BluetoothModel *m_model;
//    QList<AdapterWidget *> m_widgets;
    QMap<const Adapter*, widgets::NextPageWidget*> m_valueMap;
    widgets::SettingsGroup *m_mainGroup;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHWIDGET_H
