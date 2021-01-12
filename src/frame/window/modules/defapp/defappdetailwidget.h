/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace defapp {
class DefappDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefappDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category, QWidget *parent = nullptr);
    void setModel(dcc::defapp::DefAppModel *const model);
    void setCategory(dcc::defapp::Category *const category);

private:
    void updateListView(const dcc::defapp::App &defaultApp);
    QIcon getAppIcon(const QString &appIcon, const QSize &size);
    dcc::defapp::App getAppById(const QString &appId);
    void appendItemData(const dcc::defapp::App &app);
    bool isDesktopOrBinaryFile(const QString &fileName);
    bool isValid(const dcc::defapp::App &app);
    enum DefAppDataRole{
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
    void onDelBtnClicked();
    void onClearAll();

private:
    void AppsItemChanged(const QList<dcc::defapp::App> &list);
    void addItem(const dcc::defapp::App &item);
    void removeItem(const dcc::defapp::App &item);
    void showInvalidText(DTK_WIDGET_NAMESPACE::DStandardItem *modelItem, const QString &name, const QString &iconName);

private:
    QVBoxLayout *m_centralLayout;
    DTK_WIDGET_NAMESPACE::DListView *m_defApps;
    QStandardItemModel *m_model;
    QString m_categoryName;
    int m_categoryValue;
    dcc::defapp::Category *m_category;
    QMap<DTK_WIDGET_NAMESPACE::DViewItemAction *, QString> m_actionMap;
    int m_systemAppCnt;
    int m_userAppCnt;
};
}
}
