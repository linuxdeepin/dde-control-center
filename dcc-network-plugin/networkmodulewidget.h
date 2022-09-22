// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

namespace DCC_NAMESPACE {
  class FrameProxyInterface;
}

using namespace dde::network;
using namespace DCC_NAMESPACE;

enum class PageType {
    NonePage = -1,
    WiredPage = 0,
    WirelessPage,
    DSLPage,
    VPNPage,
    SysProxyPage,
    AppProxyPage,
    HotspotPage,
    NetworkInfoPage,
    AirplaneModepage
};

class NetworkModuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkModuleWidget(QWidget *parent = Q_NULLPTR);
    ~NetworkModuleWidget();
    void showDefaultWidget();
    int gotoSetting(const QString &path);
    void setIndexFromPath(const QString &path);
    void initSetting(const int settingIndex, const QString &searchPath);
    void setModelVisible(const QString &modelName, const bool &visible);

Q_SIGNALS:
    void requestShowVpnPage(const QString &path = "") const;
    void requestShowPppPage(const QString &path = "") const;
    void requestShowProxyPage() const;
    void requestShowInfomation() const;
    void requestShowChainsPage() const;
    void requestHotspotPage(const QString &path = "") const;
    void requestShowDeviceDetail(NetworkDeviceBase *device, const QString &path = "") const;
    void requestShowAirplanePage() const;

private Q_SLOTS:
    void onDeviceStatusChanged();
    void onDeviceChanged();
    void onProxyMethodChanged(const ProxyMethod &method);

private:
    bool handleNMEditor();
    // 设置当前索引
    void setCurrentIndex(const int settingIndex);
    void initIpConflictInfo(const QList<NetworkDeviceBase *> &devices);
    void selectListIndex(const QModelIndex &idx);

private:
    QVBoxLayout *m_centralLayout;
    dcc::widgets::MultiSelectListView *m_lvnmpages;
    QStandardItemModel *m_modelpages;
    QProcess *m_nmConnectionEditorProcess;
    QGSettings *m_settings;

    FrameProxyInterface *m_frameProxy;
    bool m_isFirstEnter;
    bool m_switchIndex;
};

Q_DECLARE_METATYPE(PageType)
#endif // NETWORKMODULEWIDGET_H
