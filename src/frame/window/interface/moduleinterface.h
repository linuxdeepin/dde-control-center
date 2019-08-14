/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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

#pragma once

#include "frameproxyinterface.h"
#include "window/namespace.h"

#include <QtCore>

//struct ModuleMetadata {
//    QString icon;
//    QString title;
//};

namespace DCC_NAMESPACE {

// ModuleInterface作为每个规范每个Module的接口，每个Module实现必须实现其所有虚函数。
class ModuleInterface
{
public:
    ModuleInterface(FrameProxyInterface *frameProxy) : m_frameProxy(frameProxy) {}
    virtual ~ModuleInterface() {}

    // preInitialize会在模块初始化时被调用，用于模块在准备阶段进行资源的初始化；
    // preInitialize不允许进行高资源的操作；
    virtual void preInitialize() {}

    // initialize初始化相应的模块，参数proxy用于Moudle向Frame信息查询和主动调用；
    // 返回Module的id；
    // initialize的时候不能做资源占用较高的操作；
    virtual void initialize() = 0;

    // reset module settings
    virtual void reset() {}

    ///
    /// \brief name
    /// your module name
    /// \return
    ///
    virtual const QString name() const = 0;

    // 应该暂时不需要finalize；
    // virtual void finalize();

    // 获取Module的Metadata;
    // virtual ModuleMetadata getMetadata();

    ///
    /// \brief showPage
    /// show specified module page
    /// \param pageName
    /// the page name
    ///
    virtual void showPage(const QString &pageName) { Q_UNUSED(pageName); }

    // 返回模块主Widget；
    virtual QWidget *moduleWidget() { return nullptr;}

    ///
    /// \brief contentPopped
    /// call when specific widget popped
    /// \param w
    ///
    virtual void contentPopped(QWidget *const w) { Q_UNUSED(w);}

    ///
    /// \brief active
    /// 当模块第一次被点击进入时，active会被调用
    virtual void active();

    ///
    /// \brief active
    /// 当模块被销毁时，deactive会被调用
    virtual void deactive();

protected:
    FrameProxyInterface *m_frameProxy = nullptr;
};

}
