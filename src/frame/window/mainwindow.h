/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "navigation/navmodel.h"
#include "interface/frameproxyinterface.h"
#include "interface/moduleinterface.h"

#include <DMainWindow>
#include <DBackgroundGroup>
#include <DListView>
#include <DIconButton>

#include <QStack>
#include <QPair>
#include <QDBusContext>
#include <QGSettings>

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
    void showModulePage(const QString &module, const QString &page, bool animation) override;
    void setModuleSubscriptVisible(const QString &module, bool bIsDisplay) override;
    void setRemoveableDeviceStatus(QString type, bool state) override;
    bool getRemoveableDeviceStatus(QString type) const override;

public:
    bool isModuleAvailable(const QString &m);
    void toggle();
    void popWidget();
    void initAllModule(QString m = "");

Q_SIGNALS:
    void moduleVisibleChanged(const QString &module, bool visible);

private:
    void changeEvent(QEvent *event) override;

private Q_SLOTS:
    void onEnterSearchWidget(QString moduleName, QString widget);
    void onBack();
    void resetTabOrder();
    void findFocusChild(QWidget *w, QWidget *&pre);
    void findFocusChild(QLayout *l, QWidget *&pre);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void resetNavList(bool isIconMode);
    void modulePreInitialize(QString m = nullptr);
    void popAllWidgets(int place = 0);//place is Remain count
    void onFirstItemClick(const QModelIndex &index);
    void pushNormalWidget(ModuleInterface *const inter, QWidget *const w);  //exchange third widget : push new widget
    void replaceThirdWidget(ModuleInterface *const inter, QWidget *const w);  //replace(hide) third widget : Can recover
    void pushTopWidget(ModuleInterface *const inter, QWidget *const w);  //Covere the top
    void pushFinalWidget(ModuleInterface *const inter, QWidget *const w);  //Insert after the finalWidget
    void judgeTopWidgetPlace(ModuleInterface *const inter, QWidget *const w);
    void updateViewBackground();
    void loadModules();

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
};
}

#endif // MAINWINDOW_H
