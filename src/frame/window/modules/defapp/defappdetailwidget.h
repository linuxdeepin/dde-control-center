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

#include "window/namespace.h"
#include "modules/defapp/defappworker.h"

#include <dtkwidget_global.h>

#include <QWidget>

namespace dcc {
namespace defapp {
class DefAppModel;
class Category;
struct App;
}
}

DWIDGET_BEGIN_NAMESPACE
class DFloatingButton;
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
class DefAppListView;
class DelAppDelegate;
class DefappDetailWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DefappDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category, QWidget *parent = nullptr);
    void setModel(dcc::defapp::DefAppModel *const model);
    void setCategory(dcc::defapp::Category *const category);

private:
    void updateListView(const dcc::defapp::App &app);
    QIcon getAppIcon(const dcc::defapp::App &app);
    dcc::defapp::App getAppById(const QString &appId);
    void appendItemData(const dcc::defapp::App &app);
    bool isDesktopOrBinaryFile(const QString &fileName);

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
    void onDelBtnClicked(const QModelIndex &index);

private:
    void AppsItemChanged(const QList<dcc::defapp::App> &list);
    void addItem(const dcc::defapp::App &item);
    void removeItem(const dcc::defapp::App &item);

private:
    QVBoxLayout *m_centralLayout;
    DefAppListView *m_defApps;
    QStandardItemModel *m_model;
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_addBtn;
    QString m_categoryName;
    int m_categoryValue;
    dcc::defapp::Category *m_category;
    DelAppDelegate *m_delItemDelegate;
};
}
}
