// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/defapp/defappworker.h"

#include <DStyleOption>

#include <QWidget>
#include <QMap>

namespace dcc {
namespace defapp {
class DefAppModel;
class Category;
struct App;
}
}

DWIDGET_BEGIN_NAMESPACE
class DFloatingButton;
class DListView;
class DViewItemAction;
class DStandardItem;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItem;
class QPushButton;
class QStandardItemModel;
class QVBoxLayout;
class QFileInfo;
class QIcon;
class QFileDialog;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace defapp {
class DefappDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefappDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category, QWidget *parent = nullptr);
    virtual ~DefappDetailWidget();
    void setModel(dcc::defapp::DefAppModel *const model);
    void setCategory(dcc::defapp::Category *const category);

private:
    void updateListView(const dcc::defapp::App &defaultApp);
    QIcon getAppIcon(const QString &appIcon, const QSize &size);
    dcc::defapp::App getAppById(const QString &appId);
    void appendItemData(const dcc::defapp::App &app);
    bool isDesktopOrBinaryFile(const QString &fileName);
    bool isValid(const dcc::defapp::App &app);
    enum DefAppDataRole {
        DefAppIsUserRole = DTK_NAMESPACE::UserRole + 1,
        DefAppIdRole,
        DefAppCanDeleteRole,
        DefAppNameRole,
        DefAppIconRole
    };

Q_SIGNALS:
    void requestSetDefaultApp(const QString &category, const dcc::defapp::App &item);
    void requestCreateFile(const QString &category, const QFileInfo &info);
    void requestDelUserApp(const QString &name, const dcc::defapp::App &item);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void onDefaultAppSet(const dcc::defapp::App &app);
    void setCategoryName(const QString &name);
    void onListViewClicked(const QModelIndex &index);
    void onAddBtnClicked();
    void onDelBtnClicked();
    void onClearAll();
    DTK_WIDGET_NAMESPACE::DListView *getAppListview() const { return m_defApps; }

private:
    void AppsItemChanged(const QList<dcc::defapp::App> &list);
    void addItem(const dcc::defapp::App &item);
    void removeItem(const dcc::defapp::App &item);
    void showInvalidText(DTK_WIDGET_NAMESPACE::DStandardItem *modelItem, const QString &name, const QString &iconName);

private:
    QVBoxLayout *m_centralLayout;
    DTK_WIDGET_NAMESPACE::DListView *m_defApps;
    QStandardItemModel *m_model;
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_addBtn;
    QString m_categoryName;
    int m_categoryValue;
    dcc::defapp::Category *m_category;
    QMap<DTK_WIDGET_NAMESPACE::DViewItemAction *, QString> m_actionMap;
    int m_systemAppCnt;
    int m_userAppCnt;
    QFileDialog *m_createFile;
};
}
}
