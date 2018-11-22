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

#ifndef MODULEINTERFACE_H
#define MODULEINTERFACE_H

#include "frameproxyinterface.h"

#include <QtCore>

namespace dcc {
class ModuleWidget;
class ContentWidget;
}
//struct ModuleMetadata {
//    QString icon;
//    QString title;
//};

namespace dcc {

// ModuleInterface作为每个规范每个Module的接口，每个Module实现必须实现其所有虚函数。
class ModuleInterface
{
public:
    ModuleInterface(FrameProxyInterface *frameProxy) : m_frameProxy(frameProxy) {}
    virtual ~ModuleInterface() {}

    // initialize初始化相应的模块，参数proxy用于Moudle向Frame信息查询和主动调用；
    // 返回Module的id；
    // initialize的时候不能做资源占用较高的操作；
    virtual void initialize() = 0;

    // reset module settings
    virtual void reset() = 0;

    ///
    /// \brief name
    /// your module name
    /// \return
    ///
    virtual const QString name() const = 0 ;

    // activate通知Moudule开始请求数据和加载内容；
    // 有两种情况需要考虑，一是模块未初始化，二是模块从deactivated状态恢复，两种情况
    // 都要重新获取数据；
//    virtual void activate();
    virtual void moduleActive() = 0;

    // deactivate通知模块进入不活跃状态；
    // 禁止所有数据请求、后端调用；
//    virtual void deactivate();
    virtual void moduleDeactive() = 0;

    // 应该暂时不需要finalize；
    // virtual void finalize();

    // 获取Module的Metadata;
//    virtual ModuleMetadata getMetadata();

    ///
    /// \brief showPage
    /// show specified module page
    /// \param pageName
    /// the page name
    ///
    virtual void showPage(const QString &pageName) { Q_UNUSED(pageName); }

    // 返回模块主Widget；
    virtual ModuleWidget *moduleWidget() = 0;

    ///
    /// \brief contentPopped
    /// call when specific widget popped
    /// \param w
    ///
    virtual void contentPopped(ContentWidget * const w) = 0;

protected:
    FrameProxyInterface *m_frameProxy = nullptr;
};

}

#endif // MODULEINTERFACE_H
