/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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

#include "modules/notification/notificationmodel.h"

#include <dtkwidget_global.h>

#include <QWidget>
#include <QModelIndex>

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QVBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace notification {

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(dcc::notification::NotificationModel *model, QWidget *parent = nullptr);
    void setModel(dcc::notification::NotificationModel *model);
    QIcon getAppIcon(const QString &appIcon, const QSize &size);

Q_SIGNALS:
    void requestShowSystem();
    void requestShowApp(int);

public Q_SLOTS:
    void onAppClicked(const QModelIndex &index);
    void setAppCurrentIndex(int row);
    void onSystemClicked(const QModelIndex &index);
    void refreshList();
private:
    Dtk::Widget::DListView *m_softwareListView;
    Dtk::Widget::DListView *m_systemListView;
    QStandardItemModel *m_sysmodel;
    QStandardItemModel *m_softwaremodel;
    QVBoxLayout *m_centralLayout;
    dcc::notification::NotificationModel  *m_model;
    QString m_theme;
    QModelIndex m_lastIndex;
};

}
}
