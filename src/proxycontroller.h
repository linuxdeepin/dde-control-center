/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#ifndef PROXYCONTROLLER_H
#define PROXYCONTROLLER_H

#include "networkconst.h"

#include <QObject>

namespace dde {

namespace network {

class ProxyController : public QObject
{
    Q_OBJECT

    friend class NetworkController;

public:
    /* 系统代理 */
    void querySysProxyData();                                               // 查询系统代理，一般在控制中心点击的时候实时调用
    void setProxyMethod(const ProxyMethod &pm);                             // 设置代理模式
    inline ProxyMethod proxyMethod() const { return m_proxyMothod; }        // 返回代理模式
    void setProxyIgnoreHosts(const QString &hosts);                         // 设置忽略的主机
    inline QString proxyIgnoreHosts() const { return m_proxyIgnoreHosts; }  // 返回代理主机列表
    void setAutoProxy(const QString &proxy);                                // 设置自动代理的配置URL
    inline QString autoProxy() { return m_autoProxyURL; }                   // 返回自动代理的配置URL
    void setProxy(const SysProxyType &type,
                  const QString &addr, const QString &port);                // 设置代理信息，type是一个枚举类型，一般是manual模式下设置
    SysProxyConfig proxy(const SysProxyType &type) const;                   // 获取对应类型下的系统代理数据

    /* 应用代理 */
    bool appProxyExist() const;
    void setAppProxy(const AppProxyConfig &config);                         // 设置应用代理数据
    AppProxyConfig appProxy() const;                                        // 获取应用代理数据

Q_SIGNALS:
    /* 系统代理 */
    void autoProxyChanged(const QString &);                                 // 自动代理发生变化
    void proxyChanged(const SysProxyConfig &);                              // 某种代理数据发生变化的时候触发的信号
    void proxyMethodChanged(const ProxyMethod &);                           // 代理模式发生变化
    void proxyIgnoreHostsChanged(const QString &);                          // 忽略的主机发生变化
    /* 应用代理 */
    void appIPChanged(const QString &ip);                                   // 应用代理IP发生变化
    void appPasswordChanged(const QString &password);                       // 应用代理密码发生变化
    void appTypeChanged(const AppProxyType &type);                          // 应用代理类型发生变化
    void appUsernameChanged(const QString &username);                       // 应用代理用户名发生变化
    void appPortChanged(const uint &port);                                  // 应用代理端口发生变化

protected:
    explicit ProxyController(NetworkInter *networkInter, QObject *parent = Q_NULLPTR);
    ~ProxyController();

    void queryAutoProxy();                                                  // 查询自动代理的URL
    void queryProxyMethod();                                                // 查询自动代理模式
    void queryProxyIgnoreHosts();                                           // 查询代理忽略的主机

    void queryProxyDataByType(const QString &type);                         // 查询代理数据

    ProxyMethod convertProxyMethod(const QString &method);                  // 将代理模式字符串转换为枚举
    QString convertProxyMethod(const ProxyMethod &method);                  // 将代理模式枚举转换为字符串

    QString convertSysProxyType(const SysProxyType &type);                  // 将代理模式枚举值转换为字符串
    SysProxyType convertSysProxyType(const QString &type);                  // 将代理模式字符串转换为枚举值

    AppProxyType appProxyType(const QString &v);                            // 系统代理类型转换，字符串到枚举
    QString appProxyType(const AppProxyType &v);                            // 系统代理类型转换，枚举到字符串

private Q_SLOTS:
    void onIPChanged(const QString &value);
    void onPasswordChanged(const QString &value);
    void onTypeChanged(const QString &value);
    void onUserChanged(const QString &value);
    void onPortChanged(uint value);

private:
    NetworkInter *m_networkInter;
    ProxyChains *m_chainsInter;
    QList<SysProxyConfig> m_sysProxyConfig;
    QString m_autoProxyURL;
    ProxyMethod m_proxyMothod;
    QString m_proxyIgnoreHosts;
    AppProxyConfig m_appProxyConfig;
    bool m_appProxyExist;
};

}

}

#endif // UPROXYCONTROLLER_H
