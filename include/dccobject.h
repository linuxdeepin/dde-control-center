// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCOBJECT_H
#define DCCOBJECT_H

#include <QObject>
#include <QQmlComponent>
#include <QQmlListProperty>
#include <QQuickItem>

namespace dccV25 {
class DccObject : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString parentName READ parentName WRITE setParentName NOTIFY parentNameChanged)
    Q_PROPERTY(quint32 weight READ weight WRITE setWeight NOTIFY weightChanged)

    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(qint8 badge READ badge WRITE setBadge NOTIFY badgeChanged DESIGNABLE false)

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged DESIGNABLE false)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged DESIGNABLE false)
    Q_PROPERTY(bool visibleToApp READ isVisibleToApp NOTIFY visibleToAppChanged) // 未设置隐藏且未被配置隐藏则为true
    Q_PROPERTY(bool enabledToApp READ isEnabledToApp NOTIFY enabledToAppChanged) // 未设置禁用且未被配置禁用则为true
    Q_PROPERTY(bool canSearch READ canSearch WRITE setCanSearch NOTIFY canSearchChanged DESIGNABLE false)

    Q_PROPERTY(BackgroundTypes backgroundType READ backgroundType WRITE setBackgroundType NOTIFY backgroundTypeChanged DESIGNABLE false)

    Q_PROPERTY(DccObject* currentObject READ currentObject WRITE setCurrentObject NOTIFY currentObjectChanged DESIGNABLE false)
    Q_PROPERTY(QVector<DccObject *> children READ getChildren NOTIFY childrenChanged DESIGNABLE false)

    Q_PROPERTY(quint8 pageType READ pageType WRITE setPageType NOTIFY pageTypeChanged)
    Q_PROPERTY(QQmlComponent* page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(QQuickItem* parentItem READ parentItem WRITE setParentItem NOTIFY parentItemChanged)

    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")

    explicit DccObject(QObject *parent = nullptr);
    ~DccObject() override;

    QString name() const;
    void setName(const QString &name);
    QString parentName() const;
    void setParentName(const QString &parentName);
    quint32 weight() const;
    void setWeight(quint32 weight);

    QString displayName() const;
    void setDisplayName(const QString &displayName);
    QString description() const;
    void setDescription(const QString &description);
    QString icon() const;
    void setIcon(const QString &icon);
    QUrl iconSource() const;
    qint8 badge() const;
    void setBadge(qint8 badge);

    bool isVisible() const;
    void setVisible(bool isVisible);
    bool isVisibleToApp() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool isEnabledToApp() const;

    // 是否参与搜索，默认参与搜索
    bool canSearch() const;
    void setCanSearch(bool canSearch);

    enum BackgroundType {
        AutoBg = 0,                              // 自动，Menu为Normal|Hover，Group为Normal，其他无背景，默认
        Normal = 0x01,                           // 正常base背景
        Hover = 0x02,                            // 鼠标悬浮背景
        Clickable = 0x04,                        // 点击触发active信号
        Highlight = 0x08,                        // 高亮背景，通常表示选中状态
        Warning = 0x10,                          // 红色
        ClickStyle = Normal | Hover | Clickable, // 有悬浮背景并可点击
    };
    Q_DECLARE_FLAGS(BackgroundTypes, BackgroundType)
    Q_FLAG(BackgroundTypes)
    BackgroundTypes backgroundType() const;
    void setBackgroundType(BackgroundTypes type);

    DccObject *currentObject();
    void setCurrentObject(DccObject *obj);

    enum PageType {
        EditorPage = 1, // 编辑控件,page为右则的编辑控件，左则为displayName和description
        ItemPage,       // 控件，page为整个控件

        Menu = 0x40, // 菜单项，子页面是page，默认
        MenuEditor,  // 菜单加编辑控件,子项是一个菜单项

        Control = 0x20,                // 页面中的一个控件，与其他组合使用
        Editor = EditorPage | Control, // 编辑控件
        Item = ItemPage | Control,     // 控件

        UserType = 0x80, // 0x80及以上: 用户自定义使用
    };
    Q_ENUM(PageType)
    quint8 pageType() const;
    void setPageType(quint8 type);
    // QT_DEPRECATED_X("use DccLoader")
    // Q_INVOKABLE QQuickItem *getSectionItem(QObject *parent);
    QQuickItem *parentItem();
    void setParentItem(QQuickItem *item);
    QQmlComponent *page() const;
    void setPage(QQmlComponent *page);

    QQmlListProperty<QObject> data();
    const QVector<DccObject *> &getChildren() const;

    class Private;

Q_SIGNALS:
    // 激活信号
    void active(const QString &cmd);
    void deactive();
    // 子项变化信号
    void childAboutToBeAdded(const DccObject *parent, int pos);
    void childAdded(DccObject *child);
    void childAboutToBeRemoved(const DccObject *parent, int pos);
    void childRemoved(DccObject *child);
    void childAboutToBeMoved(const DccObject *parent, int pos, int oldPos);
    void childMoved(DccObject *child);
    void childrenChanged(const QVector<DccObject *> &children);
    // 属性变化信号
    void nameChanged(const QString &name);
    void parentNameChanged(const QString &parentName);
    void weightChanged(quint32 weight);

    void displayNameChanged(const QString &displayName);
    void descriptionChanged(const QString &description);
    void iconChanged(const QString &icon);
    void iconSourceChanged(const QUrl &iconSource);
    void badgeChanged(qint8 badge);

    void visibleChanged(bool visible);
    void enabledChanged(bool enabled);
    void visibleToAppChanged(bool visibleToApp);
    void enabledToAppChanged(bool enabledToApp);
    void canSearchChanged(bool canSearch);

    void backgroundTypeChanged(BackgroundTypes type);

    void currentObjectChanged(DccObject *obj);

    void pageTypeChanged(quint8 type);
    void pageChanged(QQmlComponent *page);
    void parentItemChanged(QQuickItem *item);

protected:
    DccObject::Private *p_ptr;
};
} // namespace dccV25
#endif // DCCOBJECT_H
