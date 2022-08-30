// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "window/utils.h"
#include "window/insertplugin.h"

#include <DListView>

#include <QWidget>
#include <QMetaMethod>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListView;
class QStandardItemModel;
class QModelIndex;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace systeminfo {

class SystemInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SystemInfoWidget(QWidget *parent = nullptr);
    virtual ~SystemInfoWidget() override;

    void setCurrentIndex(int index);
    DTK_WIDGET_NAMESPACE::DListView *getSystemListViewPointer();
    void showDefaultWidget();
    int showPath(const QString &path);

private:
    void initWidget();
    void initData();

    void onRequestUpdateSecondMenu(int row, const QString &name);
    void onListClicked(const QModelIndex & index);
    bool configContent(const QString & configName);

Q_SIGNALS:
    void requestShowAboutNative();
    void requestShowVersionProtocol();
    void requestShowEndUserLicenseAgreement();
    void requestShowPrivacyPolicy();
    void requestUpdateSecondMenu(bool);

#ifndef DISABLE_RECOVERY
    void requestShowRestore();
#endif

private:
    QVBoxLayout *m_mainContentLayout;
    dcc::widgets::MultiSelectListView *m_listView;
    QStandardItemModel *m_itemModel;
    QList<ListSubItem> m_itemList;
    QModelIndex m_lastIndex;
};

}
}
