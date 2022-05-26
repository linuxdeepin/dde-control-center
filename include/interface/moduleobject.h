/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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

#include "namespace.h"

#include <QIcon>
#include <QDebug>
#include <DObject>

// 扩展按钮，在VList和Page布局中放在最下面，横向排列
#define DCC_EXTRA 0x00800000
#define setExtra() setFlagState(DCC_EXTRA, true)
#define extra() getFlagState(DCC_EXTRA)

using DCC_LAYOUT_TYPE = uint32_t;
namespace DCC_NAMESPACE {
class ModuleObjectPrivate;
/**
 * @brief ModuleObject作为规范每个Module的接口，每个Module必须提供其基本的信息
 */
class ModuleObject : public QObject, public DTK_CORE_NAMESPACE::DObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY moduleDataChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY moduleDataChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY moduleDataChanged)
    Q_PROPERTY(QStringList contentText READ contentText WRITE setContentText NOTIFY moduleDataChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY moduleDataChanged)
    Q_PROPERTY(int badge READ badge WRITE setBadge NOTIFY moduleDataChanged)

    Q_PROPERTY(bool hiden READ isHiden WRITE setHiden NOTIFY stateChanged)
    Q_PROPERTY(bool disabled READ isDisabled WRITE setDisabled NOTIFY stateChanged)

public:
    enum : DCC_LAYOUT_TYPE {
        Main = 0x01000000,  //主菜单显示
        HList = 0x02000000, //横向菜单列表显示
        VList = 0x03000000, //纵向菜单列表显示
        Page = 0x04000000   //页面显示
    };
    ModuleObject(QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName = {}, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QStringList &contentText, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QIcon &icon, QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QString &description, const QIcon &icon = QIcon(), QObject *parent = nullptr);
    ModuleObject(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QIcon &icon = QIcon(), QObject *parent = nullptr);

    virtual ~ModuleObject();

    /**
     * @brief 子项类型，由类型决定view显示效果
     */
    DCC_LAYOUT_TYPE childType() const;
    /**
     * @brief 设置子项类型
     */
    void setChildType(const DCC_LAYOUT_TYPE &t);
    /**
     * @brief 当进入模块时，active会被调用，如无需通知则可不实现
     */
    virtual void active() {}
    /**
     * @brief 每次被调均需new新的QWidget
     * @return 返回自定义页面
     */
    virtual QWidget *page() { return nullptr; }
    /**
     * @brief 当离开模块时，deactive会被调用，如无需通知则可不实现
     */
    virtual void deactive() {}

public:
    QString name() const;
    QString displayName() const;
    QString description() const;
    QStringList contentText() const;
    QIcon icon() const;
    int badge() const;

    bool isHiden() const;
    bool isDisabled() const;
    /**
     * @brief 获取状态标志
     * @return 对应状态位是否
     */
    bool getFlagState(uint32_t flag) const;
    uint32_t getFlag() const;

    /**
     * @brief setFlagState 设置状态标志，状态标志共32位，高16位为预留，低16位可根据需要设置
     * @param flag 需要设置的状态位
     * @param state true 置位 false 复位
     */
    void setFlagState(uint32_t flag, bool state);

public Q_SLOTS:
    void setHiden(bool hiden);
    void setDisabled(bool disabled);
    void trigger();

    // 名称，作为每个模块的唯一标识，不可为空
    void setName(const QString &name);
    // 显示名称，如菜单的名称，页面的标题等，为空则不显示
    void setDisplayName(const QString &displayName);
    // 描述，如主菜单的描述信息
    void setDescription(const QString &description);
    // 上下文数据，参与搜索，只可用于终结点：DisplayName -> ContentText(one of it)
    void setContentText(const QStringList &contentText);
    void addContentText(const QString &contentText);
    void addContentText(const QStringList &contentText);
    // 图标，如主菜单的图标
    void setIcon(const QIcon &icon);
    // 主菜单中的角标, 默认为0不显示，大于0显示
    void setBadge(int badge);

Q_SIGNALS:
    /**
     * @brief 基本信息改变后发送此信号
     */
    void moduleDataChanged();
    /**
     * @brief stateChanged 状态标志变化 (可见、禁用等)
     * @param flag
     * @param state
     */
    void stateChanged(uint32_t flag, bool state);
    /**
     * @brief childStateChanged 子项状态标志变化 (可见、禁用等)
     * @param child 子项
     * @param flag 标志
     * @param state 变化后值
     */
    void childStateChanged(ModuleObject *const child, uint32_t flag, bool state);

    /**
     * @brief 添加child后触发
     */
    void appendedChild(ModuleObject *const module);
    /**
     * @brief 删除child前触发
     */
    void removedChild(ModuleObject *const module);
    /**
     * @brief 插入child后触发
     */
    void insertedChild(ModuleObject *const module);
    /**
     * @brief childrens() 改变后必须发送此信号
     */
    void childrenSizeChanged(const int size);
    /**
     * @brief trigger触发该信号，框架收到信号会切换到该ModuleObject页
     */
    void triggered();

public:
    ModuleObject *getParent();
    /**
     * @brief 搜索子项，使用广度搜索
     * @param child 子项
     * @return 子项所在的层数，返回0表示module自己，-1表示未搜索到
     */
    int findChild(ModuleObject *const child);

    static int findChild(ModuleObject *const module, ModuleObject *const child);

    inline bool hasChildrens() { return !childrens().isEmpty(); }

    /**
     * @brief 子项，不可直接使用QList进行增删改操作，应使用appendChild、removeChild、insertChild
     * @return 子项列表
     */
    const QList<ModuleObject *> &childrens();
    ModuleObject *children(const int index) const;
    int getChildrenSize() const;

    void appendChild(ModuleObject *const module);
    void removeChild(ModuleObject *const module);
    void removeChild(const int index);
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module);
    void insertChild(const int index, ModuleObject *const module);

    //! Returns current ModuleObject version
    static unsigned GetCurrentVersion();

private:
    static int findChild(ModuleObject *const module, ModuleObject *const child, const int num);

private:
    D_DECLARE_PRIVATE(ModuleObject)
    Q_DISABLE_COPY(ModuleObject)
};

} // namespace DCC_NAMESPACE
