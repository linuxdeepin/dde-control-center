#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <QObject>
#include "interface/namespace.h"
#include "interface/moduleinterface.h"
#include "interface/frameproxyinterface.h"

class NetworkModuleWidget;
class WirelessPage;
class ConnectionEditPage;

namespace dde {
  namespace network {
    class NetworkDeviceBase;
  }
}

namespace DCC_NAMESPACE {
  class ModuleInterface;
  class FrameProxyInterface;
}

using namespace DCC_NAMESPACE;
using namespace dde::network;

class DCCNetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID ModuleInterface_iid)
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)

public:
    explicit DCCNetworkModule();

    ~DCCNetworkModule() Q_DECL_OVERRIDE;

    // 初始化函数
    void initialize() Q_DECL_OVERRIDE;

    void active() Q_DECL_OVERRIDE;

    // 返回插件名称，用于显示
    const QString displayName() const Q_DECL_OVERRIDE;

    // 返回插件图标，用于显示
    QIcon icon() const Q_DECL_OVERRIDE;

    // 返回插件翻译文件路径，用于搜索跳转
    QString translationPath() const Q_DECL_OVERRIDE;

    // 告知控制中心插件级别（一级模块还是二级菜单），必须实现
    QString path() const Q_DECL_OVERRIDE;

    // 告知控制中心插件插入位置，必须实现
    QString follow() const Q_DECL_OVERRIDE;

    const QString name() const Q_DECL_OVERRIDE;

    void showPage(const QString &pageName) Q_DECL_OVERRIDE;

    QWidget *moduleWidget() Q_DECL_OVERRIDE;

private:
    void removeConnEditPageByDevice(NetworkDeviceBase *dev);

private Q_SLOTS:
    void onDeviceChanged();
    void showWirelessEditPage(NetworkDeviceBase *dev, const QString &connUuid = QString(), const QString &apPath = QString());

    void showPppPage(const QString &searchPath);
    void showVPNPage(const QString &searchPath);
    void showDeviceDetailPage(NetworkDeviceBase *dev, const QString &searchPath);
    void showChainsProxyPage();
    void showProxyPage();
    void showHotspotPage();
    void showDetailPage();

private:
    bool m_hasAp;
    bool m_hasWired;
    bool m_hasWireless;

    NetworkModuleWidget *m_indexWidget;
    ConnectionEditPage *m_connEditPage;
};

#endif // NETWORKINTERFACE_H
