//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "frameproxyinterface.h"

#include <QtCore>
#include <QIcon>
#include <QWidget>

#include <DIconTheme>

DGUI_USE_NAMESPACE

//struct ModuleMetadata {
//    QString icon;
//    QString title;
//};

#define MAINWINDOW "mainwindow"
#define DISPLAY "display"
#define DEFAPP "defapp"
#define PERSONALIZATION "personalization"
#define COMMONINFO "commoninfo"
#define SOUND "sound"
#define DATETIME "datetime"
#define POWER "power"
#define KEYBOARD "keyboard"
#define SYSTEMINFO "systeminfo"
#define MOUSE "mouse"

namespace dccV20 {

// ModuleInterface_V1_0作为每个规范每个Module的接口，每个Module实现必须实现其所有虚函数。
class ModuleInterface
{
public:
    ModuleInterface() = default;
    ModuleInterface(FrameProxyInterface *frameProxy) : m_frameProxy(frameProxy) {}
    virtual ~ModuleInterface() {}

    void setFrameProxy(FrameProxyInterface *frameProxy) { m_frameProxy = frameProxy; }

    // preInitialize会在模块初始化时被调用，用于模块在准备阶段进行资源的初始化；
    // preInitialize不允许进行高资源的操作；
    virtual void preInitialize(bool sync = false,FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) {Q_UNUSED(sync)}

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

    ///
    /// \brief name
    /// 模块名，用于显示
    /// \return
    ///
    virtual const QString displayName() const = 0;

    ///
    /// \brief icon
    /// get module icon path
    /// \return
    ///
    virtual QIcon icon() const {
        return DIconTheme::findQIcon(QString("dcc_nav_%1").arg(name()));
    }

    ///
    /// \brief translationPath
    /// 获取多语言文件的路径，用以搜索
    /// \return QString
    ///
    virtual QString translationPath() const {
        return QStringLiteral(":/translations/dde-control-center_%1.ts");
    }

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
    /// 当模块第一次被点击进入时，active会被调用,如果是插件,重载的时候必须声明为slots,否则加载不了
    virtual void active() {}

    ///
    /// \brief active
    /// 当模块被销毁时，deactive会被调用
    virtual void deactive() {}

    ///
    /// \brief load
    /// 当搜索到相关字段后，lead会被调用
    /// 如果可以正常显示则返回 0, 否则返回非0
    virtual int load(const QString &path) {
        Q_UNUSED(path);
        return 0;
    }

    virtual QStringList availPage() const { return QStringList(); }

    /**
     * @brief path
     * @return 插件级别及二级菜单插件所属模块
     */
    virtual QString path() const {
        return QString();
    }

    /**
     * @brief follow
     * @return 插件插入位置，可以字符串或者数字
     */
    virtual QString follow() const {
        return QString();
    }

    /**
     * @brief enabled
     * @return 插件是否处于可用状态
     */
    virtual bool enabled() const {
        return true;
    }

    /**
     * @brief addChildPageTrans
     * @return 添加插件二级菜单翻译内容
     */
    virtual void addChildPageTrans() const {};

public:
    inline void setAvailable(bool isAvailable) { m_available = isAvailable; }
    inline bool isAvailable() const { return m_available; }
    inline void setEnabled(bool value) {
        QWidget *mainwindow = dynamic_cast<QWidget *>(m_frameProxy);
        if (mainwindow) {
            mainwindow->setEnabled(value);
        }
    }

    inline bool deviceUnavailabel() const
    {
        return m_deviceUnavailabel;
    }

    inline void setDeviceUnavailabel(bool deviceUnavailabel)
    {
        m_deviceUnavailabel = deviceUnavailabel;
    }

protected:
    FrameProxyInterface *m_frameProxy{nullptr};
    bool m_available{true};
    bool m_deviceUnavailabel{false};

private:
    virtual void initSearchData() {};
};

}

#define ModuleInterface_V1_0_iid "com.deepin.dde.ControlCenter.module/1.0"
Q_DECLARE_INTERFACE(dccV20::ModuleInterface, ModuleInterface_V1_0_iid)
