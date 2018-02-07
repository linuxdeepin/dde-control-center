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

#ifndef WIREDPAGE_H
#define WIREDPAGE_H

#include "contentwidget.h"
#include "connectioneditpage.h"
#include "wireddevice.h"

class QPushButton;

namespace dcc {

namespace widgets {
class SettingsGroup;
class NextPageWidget;
}

namespace network {

class WiredDevice;
class NetworkModel;
class ConnectionEditPage;
class WiredPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit WiredPage(WiredDevice *dev, QWidget *parent = 0);

    void setModel(NetworkModel *model);

signals:
    void requestConnectionsList(const QString &devPath);
    void requestEditConnection(const QString &devPath, const QString &uuid) const;
    void requestCreateConnection(const QString &type, const QString &devPath) const;
    void requestActiveConnection(const QString &devPath, const QString &uuid) const;
    void requestDeleteConnection(const QString &uuid) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestNextPage(ContentWidget * const w) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

private slots:
    void initUI();

    void refreshConnectionList();
    void editConnection();
    void createNewConnection();
    void activeConnection();
    void checkActivatedConnection();
    void onDeviceStatusChanged(const WiredDevice::DeviceStatus stat);
    void onSessionCreated(const QString &sessionPath);
    void onDeviceRemoved();

private:
    WiredDevice *m_device;
    NetworkModel *m_model;

    QPointer<ConnectionEditPage> m_editPage;

    QPushButton *m_createBtn;
    widgets::SettingsGroup *m_settingsGrp;
    widgets::SettingsGroup *m_tipsGrp;

    QMap<widgets::NextPageWidget *, QString> m_connectionPath;
};

}

}

#endif // WIREDPAGE_H
