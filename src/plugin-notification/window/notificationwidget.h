//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    explicit NotificationWidget(NotificationModel *model, QStandardItemModel *softwaremodel, QWidget *parent = nullptr);
    static QIcon getAppIcon(const QString &appIcon, const QSize &size);

Q_SIGNALS:
    void requestShowSystem();
    void requestShowApp(int);

public Q_SLOTS:
    void onAppClicked(const QModelIndex &index);
    void setAppCurrentIndex(int row);
    void onSystemClicked(const QModelIndex &index);
    Dtk::Widget::DListView* getAppListview() const {return (Dtk::Widget::DListView*)m_softwareListView;}
    Dtk::Widget::DListView* getSysListview() const {return m_systemListView;}
    void showDefaultWidget();

private:
    static const QPixmap loadSvg(const QString &fileName, const QSize &size);

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
