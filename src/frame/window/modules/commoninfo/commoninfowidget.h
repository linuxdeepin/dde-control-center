// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"
#include "window/utils.h"

#include <DListView>

#include <QWidget>
#include <QMetaMethod>
#include <QGSettings>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QListView;
class QStandardItemModel;
class QModelIndex;

template <typename T>
class QList;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace commoninfo {
class CommonInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommonInfoWidget(QWidget *parent = nullptr);
    virtual ~CommonInfoWidget() override;
    dcc::widgets::MultiSelectListView *getCommonListView();
    void showDefaultWidget();
    int showPath(const QString& path);
    void setCurrentIndex(const int index);

Q_SIGNALS:
    void requestShowBootWidget();
    void requestShowDeveloperModeWidget();
    void requestShowUEPlanWidget();
    void requestShowTabletModeWidget();
    void requestUpdateSecondMenu(bool);

private:
    void initWidget();
    void initData();
    bool configContent(const QString & configName);

private:

    QVBoxLayout *m_vBoxLayout;
    dcc::widgets::MultiSelectListView *m_listView;
    QStandardItemModel *m_itemModel;
    QList<ListSubItem> m_itemList;
    QGSettings *m_moduleDevelop{nullptr};
    bool isContensServer;
    QModelIndex m_lastIndex;
};
}
}

