// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "modules/notification/notificationmodel.h"
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

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace notification {

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(dcc::notification::NotificationModel *model, QWidget *parent = nullptr);
    QIcon getAppIcon(const QString &appIcon, const QSize &size);

Q_SIGNALS:
    void requestShowSystem();
    void requestShowApp(int);
    void popWidget();

public Q_SLOTS:
    void onAppClicked(const QModelIndex &index);
    void setAppCurrentIndex(int row);
    void onSystemClicked(const QModelIndex &index);
    void refreshList();
    Dtk::Widget::DListView* getAppListview() const {return (Dtk::Widget::DListView*)m_softwareListView;}
    Dtk::Widget::DListView* getSysListview() const {return m_systemListView;}
    void showDefaultWidget();
    void onSettingChanged(const QString &key);

private:
    const QPixmap loadSvg(const QString &fileName, const QSize &size);

private:
    dcc::widgets::MultiSelectListView *m_softwareListView;
    Dtk::Widget::DListView *m_systemListView;
    QStandardItemModel *m_sysmodel;
    QStandardItemModel *m_softwaremodel;
    QVBoxLayout *m_centralLayout;
    dcc::notification::NotificationModel *m_model;
    QString m_theme;
    QModelIndex m_lastIndex;
    QGSettings *m_setting;
    QLabel *m_appTitleLable;
};

}
}
