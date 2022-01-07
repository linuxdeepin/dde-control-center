#ifndef NETWORKCONST_H
#define NETWORKCONST_H

#include <QString>
#include <QJsonObject>

namespace dde {

namespace network {

/*
 * 系统代理类型
 */
enum class SysProxyType {
    Http = 0,    // http类型
    Https,       // https类型
    Ftp,         // ftp类型
    Socks        // socks类型
};

/*
 * 应用代理类型
 */
enum class AppProxyType {
    Http = 0,      // http类型
    Socks4,        // sock4类型
    Socks5         // sock5类型
};

/*
 * 设备类型
 */
enum class DeviceType {
    Unknown = 0,      // 未知设备
    Wired,           // 有线网卡
    Wireless         // 无线网卡
};

/*
 * @brief 设备状态
 */
enum class DeviceStatus {
    Unknown         = 0,          // 未知状态
    Unmanaged       = 10,         // 未管理
    Unavailable     = 20,         // 不可用
    Disconnected    = 30,         // 断开连接
    Prepare         = 40,         // 正在准备状态
    Config          = 50,         // 正在配置连接信息
    Needauth        = 60,         // 需要认证
    IpConfig        = 70,         // 正在配置IP
    IpCheck         = 80,         // 正在进行IP检验
    Secondaries     = 90,         // 设备正在等待第二个在当前设备连接成功之前必须激活的连接(例如VPN)
    Activated       = 100,        // 激活状态，也就是连接成功
    Deactivation    = 110,        // 断开连接
    Failed          = 120,        // 连接失败
    IpConfilct      = 121         // ip冲突
};

/*
 * 连接状态
 */
enum class Connectivity {
    /* 网络连接未知。这意味着连接检查已禁用（例如，在服务器上安装）或尚未运行。
        图形外壳程序应假定Internet连接可用，并且不显示强制门户窗口。*/
    Unknownconnectivity = 0,
    /* 主机未连接到任何网络。没有活动的连接包含默认的Internet路由，因此甚至尝试进行连
        接检查也没有意义。图形shell应使用此状态来指示网络连接不可用。*/
    Noconnectivity,
    /* Internet连接被捕获的入口网关劫持。图形shell可以打开sandboxed web浏览器窗口
        （因为捕获的门户通常会尝试对https连接进行中间人攻击），以便对网关进行身份验证，
        并在浏览器窗口关闭时使用CheckConnectivity()重新触发连接检查。*/
    Portal,
    Limited,    // 主机已连接到网络，似乎无法访问完整的Internet，但尚未检测到捕获的门户
    Full        // 主机已连接到网络，并且似乎能够访问完整的Internet。
};

/***
 * 加载网络的方式
 */

enum class ServiceLoadType
{
    LoadFromInter = 0,      // 从后台加载
    LoadFromManager,        // 从NetworkManager加载
    LoadFromService         // 从后台服务加载(备用)
};

class Connection
{
    friend class ControllItems;

public:
    QString path();                        // 对应connections数据中的path
    QString uuid();                        // 对应connections数据中的uuid
    QString id();                          // 对应connections数据中的id
    QString hwAddress();                   // 对应connections数据中的hwAddress
    QString clonedAddress();               // 对应connections数据中的clonedAddress
    QString ssid();                        // 对应connections数据中的Ssid

protected:
    Connection();
    ~Connection();
    void updateConnection(const QJsonObject &data);

private:
    QJsonObject m_data;
};

class ControllItems
{
public:
    Connection *connection() const;

protected:
    ControllItems();
    virtual ~ControllItems();
    void setConnection(const QJsonObject &jsonObj);

private:
    Connection *m_connection;
};

/**
 * @brief 系统代理配置
 */
struct SysProxyConfig {
    SysProxyType type;      // 系统代理类型，是一个枚举变量(http,https,ftp,socks)
    QString url;            // 代理的URL，一般用于自动代理的URL，手动代理的代理值
    uint port;              // 代理的端口，一般是手动代理
};
/**
 * @brief 应用代理配置
 */
struct AppProxyConfig {
    AppProxyType type;      // 应用代理类型，枚举变量(http,socks4,socks5)
    QString ip;             // IP地址
    uint port;              // 端口
    QString username;       // 用户名
    QString password;       // 密码
};

enum class ProxyMethod {
    Init = -1,               // 初始化状态，用来对数据进行初始化
    None = 0,                // 没有设置，对应字符串”none”，如果设置成这个，此时是关闭代理
    Auto,                    // 自动模式,对应字符串”auto”
    Manual                   // 手动模式,对应字符串”manual”
};

/**
 * @brief 当前网络连接状态
 */
enum class ConnectionStatus {
    Unknown = 0,             // 普通状态
    Activating = 1,         // 连接中
    Activated = 2,          // 连接成功
    Deactivating = 3,       // 断开连接中
    Deactivated = 4         // 连接已断开
};

}

}

#endif // UNETWORKCONST_H
