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
    Q_PROPERTY(int weight READ weight WRITE setWeight)

    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(int badge READ badge WRITE setBadge NOTIFY badgeChanged DESIGNABLE false)

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged DESIGNABLE false)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged DESIGNABLE false)
    Q_PROPERTY(bool visibleToApp READ isVisibleToApp NOTIFY visibleToAppChanged) // 未设置隐藏且未被配置隐藏则为true
    Q_PROPERTY(bool enabledToApp READ isEnabledToApp NOTIFY enabledToAppChanged) // 未设置禁用且未被配置禁用则为true
    Q_PROPERTY(bool canSearch READ canSearch WRITE setCanSearch NOTIFY canSearchChanged DESIGNABLE false)

    Q_PROPERTY(bool hasBackground READ hasBackground WRITE setHasBackground NOTIFY hasBackgroundChanged DESIGNABLE false)

    Q_PROPERTY(DccObject* currentObject READ currentObject WRITE setCurrentObject NOTIFY currentObjectChanged DESIGNABLE false)
    Q_PROPERTY(DccObject* defultObject READ defultObject WRITE setDefultObject NOTIFY defultObjectChanged DESIGNABLE false)
    Q_PROPERTY(QVector<DccObject *> children READ getChildren NOTIFY childrenChanged DESIGNABLE false)

    Q_PROPERTY(uint pageType READ pageType WRITE setPageType NOTIFY pageTypeChanged)
    Q_PROPERTY(QQmlComponent* page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(QQuickItem* anchorsItem READ anchorsItem WRITE setAnchorsItem NOTIFY anchorsItemChanged)

    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "data")

    explicit DccObject(QObject *parent = nullptr);
    ~DccObject() override;

    ///
    /// \brief name
    /// \return 名称
    ///
    QString name() const;
    void setName(const QString &name);
    QString parentName() const;
    void setParentName(const QString &parentName);
    int weight() const;
    void setWeight(int weight);

    QString displayName() const;
    void setDisplayName(const QString &displayName);
    QString description() const;
    void setDescription(const QString &description);
    QString icon() const;
    void setIcon(const QString &icon);
    int badge() const;
    void setBadge(int badge);

    bool isVisible() const;
    void setVisible(bool isVisible);
    bool isVisibleToApp() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);
    bool isEnabledToApp() const;

    // 是否参与搜索，默认参与搜索
    bool canSearch() const;
    void setCanSearch(bool canSearch);

    bool hasBackground() const;
    void setHasBackground(bool hasBackground);

    DccObject *currentObject();
    void setCurrentObject(DccObject *obj);
    DccObject *defultObject();
    void setDefultObject(DccObject *obj);

    enum PageType {
        Control = 0x100, // 页面中的一个控件，与其他组合使用

        Menu = 0,   // 菜单项，子页面是page
        EditorPage, // 编辑控件,page为右则的编辑控件，左则为displayName和description
        ItemPage,   // 控件，page为整个控件
        SpacerItem, // 间隔，用于处理界面中的空白，通常需要关联其他项的isVisibleToApp,高度为badge值

        Editor = EditorPage | Control, // 编辑控件
        Item = ItemPage | Control,     // 控件
        UserType = 0x1000,             // 0x1000及以上: 用户自定义使用
    };
    Q_ENUM(PageType)
    uint pageType() const;
    void setPageType(uint type);
    Q_INVOKABLE QQuickItem *getSectionItem(QObject *parent = nullptr);
    QQuickItem *anchorsItem();
    void setAnchorsItem(QQuickItem *item);
    QQmlComponent *page() const;
    void setPage(QQmlComponent *page);

    QQmlListProperty<QObject> data();
    const QVector<DccObject *> &getChildren() const;

    class Private;
public Q_SLOTS:
    void trigger();

Q_SIGNALS:
    void triggered();

    void active(const QString &cmd);
    void deactive();

    void childAboutToBeAdded(const DccObject *parent, int pos);
    void childAdded(DccObject *child);
    void childAboutToBeRemoved(const DccObject *parent, int pos);
    void childRemoved(DccObject *child);
    void childrenChanged(const QVector<DccObject *> &children);

    void nameChanged(const QString &name);
    void parentNameChanged(const QString &parentName);

    void displayNameChanged(const QString &displayName);
    void descriptionChanged(const QString &description);
    void iconChanged(const QString &icon);
    void badgeChanged(int badge);

    void visibleChanged(bool visible);
    void enabledChanged(bool enabled);
    void visibleToAppChanged(bool visibleToApp);
    void enabledToAppChanged(bool enabledToApp);
    void canSearchChanged(bool canSearch);

    void hasBackgroundChanged(bool hasBackground);

    void currentObjectChanged(DccObject *obj);
    void defultObjectChanged(DccObject *obj);

    void pageTypeChanged(uint type);
    void pageChanged(QQmlComponent *page);
    void anchorsItemChanged(QQuickItem *item);

protected:
    DccObject::Private *p_ptr;
};
} // namespace dccV25
#endif // DCCOBJECT_H
