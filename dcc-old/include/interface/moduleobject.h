// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_MODULEOBJECT_H
#define DCC_MODULEOBJECT_H

#include "namespace.h"

#include <QIcon>
#include <QObject>
#include <QVariant>

using DCC_MODULE_TYPE = uint32_t;

#define DCC_MODULE_HIERARCHY_BIT 0x80000000
#define DCC_MODULE_GROUP_BIT 0x40000000
#define DCC_MODULE_LEAF_BIT 0x20000000
#define DCC_MODULE_LAYOUT_BIT 0x10000000

#define DCC_MODULE_LISTVIEW_BIT 0x08000000
#define DCC_MODULE_ITEM_BIT 0x04000000
#define DCC_MODULE_SETTINGSGROUP_BIT 0x02000000
#define DCC_MODULE_HORIZONTAL_BIT 0x01000000
#define DCC_MODULE_VERTICAL_BIT 0x00800000

#define DCC_MODULE_MAIN_BIT 0x00040000
#define DCC_MODULE_CUSTOM_BIT 0x00020000

namespace DCC_NAMESPACE {
enum : DCC_MODULE_TYPE {
    HIERARCHY_OBJECT = DCC_MODULE_HIERARCHY_BIT,
    LEAF_OBJECT = DCC_MODULE_LEAF_BIT,
    LISTVIEW = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_LISTVIEW_BIT,
    ITEM = DCC_MODULE_LEAF_BIT | DCC_MODULE_ITEM_BIT,
    SETTINGSGROUP = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_SETTINGSGROUP_BIT,
    HORIZONTAL = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_HORIZONTAL_BIT,
    MAINLAYOUT = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_MAIN_BIT,
    HLISTLAYOUT = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_HORIZONTAL_BIT
            | DCC_MODULE_LISTVIEW_BIT,
    VLISTLAYOUT = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_VERTICAL_BIT
            | DCC_MODULE_LISTVIEW_BIT,
    PAGELAYOUT = DCC_MODULE_GROUP_BIT | DCC_MODULE_LAYOUT_BIT | DCC_MODULE_VERTICAL_BIT,
    CUSTOM_OBJECT = DCC_MODULE_CUSTOM_BIT,
};

class ModuleObjectPrivate;

/**
 * @brief ModuleInitContext 作为初始化传入的结构体，其中name和displayName 必须有数值
 * 这个结构体保存了所有初始化的信息
 */

struct ModuleInitContext
{
    QString name;
    QString displayName;
    QStringList contentText;
    QVariant icon;
    QString description;
};

/**
 * @brief ModuleObject作为规范每个Module的接口，每个Module必须提供其基本的信息
 */
class ModuleObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY moduleDataChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY moduleDataChanged)
    Q_PROPERTY(
            QStringList contentText READ contentText WRITE setContentText NOTIFY moduleDataChanged)
    Q_PROPERTY(QVariant icon READ icon WRITE setIcon NOTIFY moduleDataChanged)
    Q_PROPERTY(int badge READ badge WRITE setBadge NOTIFY moduleDataChanged)

    Q_PROPERTY(bool hidden READ isHidden WRITE setHidden NOTIFY stateChanged)
    Q_PROPERTY(bool disabled READ isDisabled WRITE setDisabled NOTIFY stateChanged)

public:
    explicit ModuleObject(QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName = {},
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QStringList &contentText,
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName,
                          const QStringList &contentText,
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName,
                          const QVariant &icon,
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName,
                          const QString &description,
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName,
                          const QString &description,
                          const QVariant &icon,
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName,
                          const QString &description,
                          const QIcon &icon,
                          QObject *parent = nullptr);
    explicit ModuleObject(const QString &name,
                          const QString &displayName,
                          const QString &description,
                          const QStringList &contentText,
                          const QVariant &icon,
                          QObject *parent = nullptr);
    explicit ModuleObject(const ModuleInitContext &message, QObject *parent = nullptr);
    virtual ~ModuleObject();
    /**
     * @brief activePage 激活并返回page
     * @param autoActive 是否自动调用active
     * @return ModuleObject隐藏时返回nullptr,否则返回page返回的QWidget *
     */
    virtual QWidget *activePage(bool autoActive = true);

public Q_SLOTS:
    /**
     * @brief 当进入模块时，active会被调用，如无需通知则可不实现
     */
    virtual void active();
    /**
     * @brief 每次被调均需new新的QWidget
     * @return 返回自定义页面
     * @warning
     * page返回的widget生命周期只是对应窗口显示的时候，即模块切换时就会被析构。ModuleObject的生命周期是从控制中心启动到关闭
     */
    virtual QWidget *page();
    /**
     * @brief 当离开模块时，deactive会被调用，如无需通知则可不实现
     */
    virtual void deactive();

public:
    QString name() const;
    QString displayName() const;
    QString description() const;
    QStringList contentText() const;
    QVariant icon() const;
    int badge() const;

    bool isHidden() const;
    bool isVisible() const;
    bool isDisabled() const;
    bool isEnabled() const;
    /**
     * @brief 获取状态标志
     * @return 对应状态位是否
     */
    bool getFlagState(uint32_t flag) const;
    uint32_t getFlag() const;

    // 扩展标志，在VList和Page布局中放在最下面，横向排列
    bool extra() const;
    void setExtra(bool value = true);

    // 是否参与搜索，默认参与搜索
    bool noSearch() const;
    void setNoSearch(bool noSearch = true);

    /**
     * @brief currentModule     当前active的子项
     *      当前active的子项变化时会触发currentModuleChanged信号，
     *      page与子项相关时需要处理currentModuleChanged信号
     * @return 当前active的子项
     */
    ModuleObject *currentModule() const;
    /**
     * @brief defultModule 默认active的子项
     *      当某个页面被active时，会通过defultModule去active子项
     *      当返回为nullptr时，不再递归active子项
     *      ModuleObject类默认处理为返回第一个未隐藏的子项
     * @return 默认active的子项
     */
    virtual ModuleObject *defultModule();

public Q_SLOTS:
    void setHidden(bool hidden);
    void setVisible(bool visible);
    void setDisabled(bool disabled);
    void setEnabled(bool enabled);
    void trigger();

    // 名称，作为每个模块的唯一标识，不可为空
    virtual void setName(const QString &name);
    // 显示名称，如菜单的名称，页面的标题等，为空则不显示
    virtual void setDisplayName(const QString &displayName);
    // 描述，如主菜单的描述信息
    virtual void setDescription(const QString &description);
    // 上下文数据，参与搜索，只可用于终结点：DisplayName -> ContentText(one of it)
    virtual void setContentText(const QStringList &contentText);
    virtual void addContentText(const QString &contentText);
    virtual void addContentText(const QStringList &contentText);
    // 图标，如主菜单的图标
    // 字符串类型,支持DDciIcon\QVariant,优先作为DDciIcon处理
    virtual void setIcon(const QVariant &icon);
    virtual void setIcon(const QIcon &icon);
    // 主菜单中的角标, 默认为0不显示，大于0显示
    virtual void setBadge(int badge);
    /**
     * @brief setFlagState 设置状态标志，状态标志共32位，高16位为预留，低16位可根据需要设置
     * @param flag 需要设置的状态位
     * @param state true 置位 false 复位
     */
    virtual void setFlagState(uint32_t flag, bool state);
    /**
     * @brief setCurrentModule 设置当前active的子项
     * @param child 当前active的子项
     */
    void setCurrentModule(ModuleObject *child);

Q_SIGNALS:
    /**
     * @brief 基本信息改变后发送此信号
     */
    void moduleDataChanged();
    /**
     * @brief displayName改变后发送此信号
     */
    void displayNameChanged(const QString &displayName);
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
    /**
     * @brief currentModuleChanged 当前激活的子项改变时会触发此信号
     * @param currentModule 当前激活的子项
     */
    void currentModuleChanged(ModuleObject *currentModule);

    void visibleChanged();

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

    virtual void appendChild(ModuleObject *const module);
    virtual void removeChild(ModuleObject *const module);
    virtual void removeChild(const int index);
    virtual void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module);
    virtual void insertChild(const int index, ModuleObject *const module);

    //! Returns current ModuleObject version
    static unsigned GetCurrentVersion();
    /**
     * @brief IsVisible 返回module是否显示，判断了配置项和程序设置项
     * @param module
     * @return
     */
    static bool IsVisible(DCC_NAMESPACE::ModuleObject *const module);
    static bool IsHidden(DCC_NAMESPACE::ModuleObject *const module);
    /**
     * @brief IsHiddenFlag 判断标志是否为隐藏标志
     * @param flag 标志
     * @return true 有隐藏标志位 false 没有隐藏标志位
     */
    static bool IsHiddenFlag(uint32_t flag);
    /**
     * @brief IsEnabled 返回module是否可用，判断了配置项和程序设置项
     * @param module
     * @return
     */
    static bool IsEnabled(DCC_NAMESPACE::ModuleObject *const module);
    static bool IsDisabled(DCC_NAMESPACE::ModuleObject *const module);
    /**
     * @brief IsDisabledFlag 判断标志是否为禁用标志
     * @param flag 标志
     * @return true 有禁用标志位 false 没有禁用标志位
     */
    static bool IsDisabledFlag(uint32_t flag);

    inline virtual DCC_MODULE_TYPE getClassID() const { return HIERARCHY_OBJECT; }

private:
    static int findChild(ModuleObject *const module, ModuleObject *const child, const int num);
    DCC_DECLARE_PRIVATE(ModuleObject)
};

} // namespace DCC_NAMESPACE

#endif // DCC_MODULEOBJECT_H
