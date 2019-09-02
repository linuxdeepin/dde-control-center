/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include <DListView>

#include <QWidget>
#include <QMetaMethod>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListView;
class QStandardItemModel;
class QModelIndex;

template <typename T>
class QList;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommonInfoWidget(QWidget *parent = nullptr);
    DTK_WIDGET_NAMESPACE::DListView *getCommonListView();
Q_SIGNALS:
    void requestShowBootWidget();
    void requestShowDeveloperModeWidget();
    void requestShowUEPlanWidget();
    void requestShowTabletModeWidget();

private:
    void initWidget();
    void initData();

private:
    struct ListSubItem {
        QString itemIcon;
        QString itemText;
        QMetaMethod itemSignal;
    };
    QVBoxLayout *m_vBoxLayout;
    DTK_WIDGET_NAMESPACE::DListView *m_listView;
    QStandardItemModel *m_itemModel;
    QList<ListSubItem> m_itemList;
};
}
}

