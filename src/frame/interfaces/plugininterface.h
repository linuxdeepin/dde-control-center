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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "frameproxyinterface.h"

#include <QtCore>

using namespace dcc;

struct ModuleMetadata {
    QString icon;
    QString title;
};

// ModuleInterface作为每个规范每个Module的接口，每个Module实现必须实现其所有虚函数。
class PluginInterface
{
public:
    // initialize初始化相应的模块，参数proxy用于Moudle向Frame信息查询和主动调用；
    // 返回Module的id；
    // initialize的时候不能做资源占用较高的操作；
    virtual void initialize(FrameProxyInterface *proxy) = 0;

    // activate通知Moudule开始请求数据和加载内容；
    // 有两种情况需要考虑，一是模块未初始化，二是模块从deactivated状态恢复，两种情况
    // 都要重新获取数据；
//    virtual void activate();

    // deactivate通知模块进入不活跃状态；
    // 禁止所有数据请求、后端调用；
//    virtual void deactivate();

    // 应该暂时不需要finalize；
    // virtual void finalize();

    // 获取Module的Metadata;
//    virtual ModuleMetadata getMetadata();

    // 返回模块主Widget；
    virtual QWidget *centralWidget() = 0;

    // widgetPoppedEvent通知Module子级控件被弹出，页面回到了第二级；
    // Module需要把控件销毁或者置为不活跃状态；
//    virtual void widgetPoppedEvent(QWidget * w);

protected:
    FrameProxyInterface *m_proxyInter = nullptr;
};

QT_BEGIN_NAMESPACE
#define ModuleInterface_iid "com.deepin.dock.PluginsItemInterface"

Q_DECLARE_INTERFACE(PluginInterface, ModuleInterface_iid)
QT_END_NAMESPACE


#endif // PLUGININTERFACE_H
