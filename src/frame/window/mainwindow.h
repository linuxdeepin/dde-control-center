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

#include <QStack>
#include <QPair>

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

namespace DCC_NAMESPACE {
class MainWindow : public DMainWindow, public FrameProxyInterface
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    void popWidget(ModuleInterface *const inter) override;
    void pushWidget(ModuleInterface *const inter, QWidget *const w, PushType type = Normal) override;
    void setModuleVisible(ModuleInterface *const inter, const bool visible) override;
    void showModulePage(const QString &module, const QString &page, bool animation) override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void resetNavList(bool isIconMode);

private:
    QHBoxLayout *m_contentLayout;
    QHBoxLayout *m_rightContentLayout;
    DListView *m_navView;
    DBackgroundGroup *m_rightView;
    QStandardItemModel *m_navModel;
    QStack<QPair<ModuleInterface *, QWidget *>> m_contentStack;
    QList<QPair<ModuleInterface *, QString>> m_modules;
    QList<ModuleInterface *> m_initList;
    QPair<ModuleInterface *, QWidget *> m_lastThirdPage;
    bool m_bIsFinalWidget;//used to distinguish the widget is final or top : fianl pop in popWidget , top pop by m_topWidget
    bool m_bIsFromSecondAddWidget;//used to save the third widget is load from final widget
    QWidget *m_topWidget;
    DCC_NAMESPACE::search::SearchWidget *m_searchWidget;

Q_SIGNALS:
    void moduleVisibleChanged(const QString &module, bool visible);

private Q_SLOTS:
    void onEnterSearchWidget(QString moduleName, QString widget);

private:
    void initAllModule();
    void modulePreInitialize();
    void popWidget();
    void popAllWidgets(int place = 0);//place is Remain count
    void onFirstItemClick(const QModelIndex &index);
    void pushNormalWidget(ModuleInterface *const inter, QWidget *const w);  //exchange third widget : push new widget
    void replaceThirdWidget(ModuleInterface *const inter, QWidget *const w);  //replace(hide) third widget : Can recover
    void pushTopWidget(ModuleInterface *const inter, QWidget *const w);  //Covere the top
    void pushFinalWidget(ModuleInterface *const inter, QWidget *const w);  //Insert after the finalWidget
    void linkReplaceBackSignal(QString moduleName, QWidget *w);
    void judgeTopWidgetPlace(ModuleInterface *const inter, QWidget *const w);
};
}

#endif // MAINWINDOW_H
