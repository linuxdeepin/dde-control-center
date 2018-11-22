/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef MIRACASTSETTINGS_H
#define MIRACASTSETTINGS_H

#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "miracastnodevicewidget.h"
#include "miracastdevicemodel.h"
#include "miracastitem.h"
#include "miracastmodel.h"
#include "widgets/labels/normallabel.h"
#include "QVBoxLayout"

using namespace dcc::widgets;

namespace dcc {

namespace display {

class MiracastPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit MiracastPage(const QString &title, QWidget *parent = 0);
    void setModel(MiracastDeviceModel *model);

Q_SIGNALS:
    void requestDeviceEnable(const QDBusObjectPath &path, const bool enable);
    void requestDeviceConnect(const QDBusObjectPath &path, const QRect &rect);
    void requestDeviceDisConnect(const QDBusObjectPath &path);
    void requestDeviceRefreshed(const QDBusObjectPath &path, const bool enable);

private Q_SLOTS:
    void onItemAdded(const SinkInfo &info);
    void onItemRemoved(const SinkInfo &info);
    void onDeviceEnableChanged(const bool enable);
    void onRefreshed();
    void onDeviceStateChanged(const bool state);
    void onDeviceManaged(const bool managed);
    void onRefreshTimeout();
    void onSinkConnectChanged(const QDBusObjectPath &sinkPath, const bool state);

private:
    MiracastItem *itemByPath(const QString &path);

private:
    QVBoxLayout *m_mainLayout;
    MiracastNoDeviceWidget *m_nodevice;
    SettingsGroup *m_deviceGrp;
    MiracastDeviceModel *m_model;
    SwitchWidget *m_deviceSwBtn;
    NormalLabel *m_tip;
    QTimer *m_refreshTimer;
    DImageButton *m_refreshBtn;
    QMap<QString, MiracastItem*> m_deviceItemList;
};

}
}

#endif // MIRACASTSETTINGS_H
