//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <interface/moduleobject.h>
#include "interface/namespace.h"
#include "defappworker.h"

#include <DStyleOption>
#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DViewItemAction;
class DStandardItem;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QVBoxLayout;
class QIcon;
QT_END_NAMESPACE

class defappworker;
class DefappDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefappDetailWidget(DefAppWorker::DefaultAppsCategory category, QWidget *parent = nullptr);
    virtual ~DefappDetailWidget();

    void setModel(DefAppModel *const model);
    void setCategory(Category *const category);

private:
    void updateListView(const App &defaultApp);
    QIcon getAppIcon(const QString &appIcon, const QSize &size);
    App getAppById(const QString &appId);
    void appendItemData(const App &app);
    bool isDesktopOrBinaryFile(const QString &fileName);
    bool isValid(const App &app);
    enum DefAppDataRole {
        DefAppIsUserRole = DTK_NAMESPACE::UserRole + 1,
        DefAppIdRole,
        DefAppCanDeleteRole,
        DefAppNameRole,
        DefAppIconRole
    };

Q_SIGNALS:
    void requestSetDefaultApp(const QString &category, const App &item);
    void requestDelUserApp(const QString &name, const App &item);

public Q_SLOTS:
    void onDefaultAppSet(const App &app);
    void setCategoryName(const QString &name);
    void onListViewClicked(const QModelIndex &index);
    void onDelBtnClicked();
    void onClearAll();
    DTK_WIDGET_NAMESPACE::DListView *getAppListview() const { return m_defApps; }

private:
    void AppsItemChanged(const QList<App> &list);
    void addItem(const App &item);
    void removeItem(const App &item);
    void showInvalidText(DTK_WIDGET_NAMESPACE::DStandardItem *modelItem, const QString &name, const QString &iconName);

private:
    QVBoxLayout *m_centralLayout;
    DTK_WIDGET_NAMESPACE::DListView *m_defApps;
    QStandardItemModel *m_model;
    QString m_categoryName;
    int m_categoryValue;
    Category *m_category;
    QMap<DTK_WIDGET_NAMESPACE::DViewItemAction *, QString> m_actionMap;
    int m_systemAppCnt;
    int m_userAppCnt;
};
