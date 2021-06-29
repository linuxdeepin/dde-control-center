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

#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "interface/namespace.h"

#include <DStyleOption>
#include <DListView>

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
class QProcess;
QT_END_NAMESPACE

namespace dde {
namespace network {
class NetworkDevice;
class WirelessDevice;
class NetworkModel;
}
}

namespace dcc {
namespace widgets {
class SettingsGroup;
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace network {

class NetworkModuleWidget : public QWidget
{
    Q_OBJECT

public:
    enum PageType {
        NonePage = -1,
        WiredPage = 0,
        WirelessPage,
        DSLPage,
        VPNPage,
        SysProxyPage,
        AppProxyPage,
        HotspotPage,
        NetworkInfoPage
    };

    explicit NetworkModuleWidget();
    ~NetworkModuleWidget();
    void setModel(dde::network::NetworkModel *model);
    void initSetting(const int settingIndex, const QString &searchPath);
    //设置当前索引
    void setCurrentIndex(const int settingIndex);
    int gotoSetting(const QString &path);
    void setIndexFromPath(const QString &path);
    void initProxyStatus();

Q_SIGNALS:
    void requestShowVpnPage(const QString &path = "") const;
    void requestShowPppPage(const QString &path = "") const;
    void requestShowProxyPage() const;
    void requestShowInfomation() const;
    void requestShowChainsPage() const;
    void requestHotspotPage() const;
    void requestShowDeviceDetail(dde::network::NetworkDevice *device, const QString &path = "") const;
    void requestDeviceEnable(const QString &devPath, const bool enable) const;

private Q_SLOTS:
    void onDeviceListChanged(const QList<dde::network::NetworkDevice *> &devices);
    void onClickCurrentListIndex(const QModelIndex &idx);
    void onProxyMethodChanged(const QString &proxyMethod);

private:
    QStandardItem *createDeviceGroup(dde::network::NetworkDevice *dev, const int number, const bool multiple);
    bool handleNMEditor();

private:
    QVBoxLayout *m_centralLayout;
    dcc::widgets::MultiSelectListView *m_lvnmpages;
    QStandardItemModel *m_modelpages;
    QString m_strNetworkManageOutput;
    enum {
        SectionRole = Dtk::UserRole + 1,
        DeviceRole,
        SearchPath
    };
    QModelIndex m_lastIndex;
    QProcess *m_nmConnectionEditorProcess;
    int m_devCount = -1;
    int m_totalDevs = -1;
    QString m_clickPageName = "";
};
}   // namespace network
}   // namespace dcc

Q_DECLARE_METATYPE(DCC_NAMESPACE::network::NetworkModuleWidget::PageType)
#endif // NETWORKMODULEWIDGET_H
