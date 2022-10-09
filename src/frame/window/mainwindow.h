// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "interface/frameproxyinterface.h"

#include <DMainWindow>
#include <DBackgroundGroup>
#include <DListView>
#include <DIconButton>

#include <QStack>
#include <QPair>
#include <QDBusContext>
#include <QGSettings>
#include <QPointer>

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QHBoxLayout;
class QStandardItemModel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace search {
class SearchWidget;
}
}

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

extern const QByteArray ControlCenterGSettings;
extern const QString GSettinsWindowWidth;
extern const QString GSettinsWindowHeight;

namespace DCC_NAMESPACE {
class ModuleInterface;
class FourthColWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FourthColWidget(QWidget *parent = nullptr);
    void initWidget(QWidget *showWidget, ModuleInterface *module = nullptr);

    inline QWidget *curWidget() const { return m_curWidget; }
    inline ModuleInterface *curInterface() const { return m_curInterface; }
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
Q_SIGNALS:
    void signalBack();
private:
    QWidget *m_curWidget{nullptr};
    ModuleInterface *m_curInterface{nullptr};
};

class MainWindow : public DMainWindow, public FrameProxyInterface, protected QDBusContext
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

    void popWidget(ModuleInterface *const inter) override;
    void pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type = Normal) override;
    void setModuleVisible(ModuleInterface *const inter, const bool visible) override;
    void setModuleVisible(const QString &module, bool visible) override;
    void setWidgetVisible(const QString &module, const QString &widget, bool visible) override;
    void setDetailVisible(const QString &module, const QString &widget, const QString &detail, bool visible) override;
    void updateSearchData(const QString &module) override;
    void showModulePage(const QString &module, const QString &page, bool animation) override;
    void setModuleSubscriptVisible(const QString &module, bool bIsDisplay) override;
    void setRemoveableDeviceStatus(QString type, bool state) override;
    bool getRemoveableDeviceStatus(QString type) const override;
    void setSearchPath(ModuleInterface *const inter) const override;
    void addChildPageTrans(const QString &menu, const QString &tran) override;
    virtual QString moduleDisplayName(const QString &module) const override;

    QString GrandSearchSearch(const QString json);
    bool GrandSearchStop(const QString json);
    bool GrandSearchAction(const QString json);

public:
    bool isModuleAvailable(const QString &m);
    void toggle();
    void popWidget();
    void initAllModule(const QString &m = "");
    inline QStack<QPair<ModuleInterface *, QWidget *>> getcontentStack() {return m_contentStack;}
    inline QSize getLastSize() const { return m_lastSize; }
    inline void setNeedRememberLastSize(bool needRememberLastSize)  { m_needRememberLastSize = needRememberLastSize;}
    void setPrimaryScreen(QScreen *screen);
    QScreen *primaryScreen() const;

Q_SIGNALS:
    void moduleVisibleChanged(const QString &module, bool visible);
    void mainwindowStateChange(int type);

private:
    void changeEvent(QEvent *event) override;
    bool event(QEvent* event) override;

private Q_SLOTS:
    void onEnterSearchWidget(QString moduleName, QString widget);
    void onBack();
    void resetTabOrder();
    void findFocusChild(QWidget *w, QWidget *&pre);
    void findFocusChild(QLayout *l, QWidget *&pre);
    void openManual();

public Q_SLOTS:
    void updateWinsize(QRect rect = QRect(0,0,0,0));

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void resetNavList(bool isIconMode);
    void modulePreInitialize(const QString &m = nullptr);
    void popAllWidgets(int place = 0);//place is Remain count
    void onFirstItemClick(const QModelIndex &index);
    void pushNormalWidget(ModuleInterface *const inter, QWidget *const w);  //exchange third widget : push new widget
    void replaceThirdWidget(ModuleInterface *const inter, QWidget *const w);  //replace(hide) third widget : Can recover
    void pushTopWidget(ModuleInterface *const inter, QWidget *const w);  //Covere the top
    void pushFinalWidget(ModuleInterface *const inter, QWidget *const w);  //Insert after the finalWidget
    void judgeTopWidgetPlace(ModuleInterface *const inter, QWidget *const w);
    void updateViewBackground();
    void updateModuleVisible();

private:
    bool m_bInit{false};
    QHBoxLayout *m_contentLayout;
    QHBoxLayout *m_rightContentLayout;
    dcc::widgets::MultiSelectListView *m_navView;
    DBackgroundGroup *m_rightView;
    QStandardItemModel *m_navModel;
    QStack<QPair<ModuleInterface *, QWidget *>> m_contentStack;
    QList<QPair<ModuleInterface *, QString>> m_modules;
    QList<ModuleInterface *> m_initList;
    QPair<ModuleInterface *, QWidget *> m_lastThirdPage;
    bool m_bIsFinalWidget;//used to distinguish the widget is final or top : fianl pop in popWidget , top pop by m_topWidget
    bool m_bIsFromSecondAddWidget;//used to save the third widget is load from final widget
    FourthColWidget *m_topWidget;
    DCC_NAMESPACE::search::SearchWidget *m_searchWidget;
    int m_firstCount;
    QString m_widgetName;
    QString m_moduleName;
    DIconButton *m_backwardBtn;
    struct CornerItemGroup {
        QString m_name;
        int m_index;
        QPair <DViewItemAction *, DViewItemAction *> m_action;
    };
    QList<CornerItemGroup> m_remindeSubscriptList;//用于记录是否有角标, QString为模块名称，DViewItemAction为角标对象
    QList<QString> m_removeableDeviceList;//用于记录可移除设备是否当前是否存在,存在加到list，不存在从list移除
    QGSettings *m_moduleSettings{nullptr};
    QGSettings *m_versionType{nullptr};
    QStringList m_hideModuleNames;
    bool m_updateVisibale = true;
    QWidget *m_lastPushWidget{nullptr};     //用于记录最后push进来的widget控件
    QSize m_lastSize;
    bool m_needRememberLastSize = true;     //用于判断是否需要上次resize的窗口大小

    //全局搜索
    QList<QJsonObject> m_lstGrandSearchTasks;
    QPointer<QScreen> m_primaryScreen;
};
}

#endif // MAINWINDOW_H
