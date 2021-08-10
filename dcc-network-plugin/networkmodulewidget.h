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
class QGSettings;
QT_END_NAMESPACE

namespace dde {
  namespace network {
    class NetworkDeviceBase;
    class NetworkController;
    class WirelessDevice;
    class WiredDevice;
    enum class ProxyMethod;
  }
}

namespace dcc {
  namespace widgets {
    class SettingsGroup;
    class MultiSelectListView;
  }
}

using namespace dde::network;

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

    explicit NetworkModuleWidget(QWidget *parent = Q_NULLPTR);
    ~NetworkModuleWidget();
    void initSetting(const int settingIndex, const QString &searchPath);
    void showDefaultWidget();
    //设置当前索引
    void setCurrentIndex(const int settingIndex);
    int gotoSetting(const QString &path);
    void setIndexFromPath(const QString &path);

Q_SIGNALS:
    void requestShowVpnPage(const QString &path = "") const;
    void requestShowPppPage(const QString &path = "") const;
    void requestShowProxyPage() const;
    void requestShowInfomation() const;
    void requestShowChainsPage() const;
    void requestHotspotPage() const;
    void requestShowDeviceDetail(NetworkDeviceBase *device, const QString &path = "") const;
    void requestUpdateSecondMenu(bool);

private Q_SLOTS:
    void onDeviceListChanged(const QList<NetworkDeviceBase *> &devices);
    void onClickCurrentListIndex(const QModelIndex &idx);
    void onProxyMethodChanged(const ProxyMethod &method);

private:
    QStandardItem *createDeviceGroup(NetworkDeviceBase *dev, const int number, const bool multiple);
    bool handleNMEditor();
    void updateSecondMenu(int row);

private:
    QVBoxLayout *m_centralLayout;
    dcc::widgets::MultiSelectListView *m_lvnmpages;
    QStandardItemModel *m_modelpages;
    QModelIndex m_lastIndex;
    QProcess *m_nmConnectionEditorProcess;
    QGSettings *m_settings;
};

Q_DECLARE_METATYPE(NetworkModuleWidget::PageType)

#endif // NETWORKMODULEWIDGET_H
