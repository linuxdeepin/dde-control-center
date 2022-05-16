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

#include "src/plugin-notification/operation/notificationmodel.h"
#include "interface/namespace.h"

#include <dtkwidget_global.h>

#include <QWidget>
#include <QModelIndex>

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QVBoxLayout;
class QGSettings;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class MultiSelectListView;
class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(NotificationModel *model, QWidget *parent = nullptr);
    QIcon getAppIcon(const QString &appIcon, const QSize &size);

Q_SIGNALS:
    void requestShowSystem();
    void requestShowApp(int);

public Q_SLOTS:
    void onAppClicked(const QModelIndex &index);
    void setAppCurrentIndex(int row);
    void onSystemClicked(const QModelIndex &index);
    void refreshList();
    Dtk::Widget::DListView* getAppListview() const {return (Dtk::Widget::DListView*)m_softwareListView;}
    Dtk::Widget::DListView* getSysListview() const {return m_systemListView;}
    void showDefaultWidget();

private:
    const QPixmap loadSvg(const QString &fileName, const QSize &size);

private:
    MultiSelectListView *m_softwareListView;
    Dtk::Widget::DListView *m_systemListView;
    QStandardItemModel *m_sysmodel;
    QStandardItemModel *m_softwaremodel;
    QVBoxLayout *m_centralLayout;
    NotificationModel *m_model;
    QString m_theme;
    QModelIndex m_lastIndex;
    QLabel *m_appTitleLable;
};
}
