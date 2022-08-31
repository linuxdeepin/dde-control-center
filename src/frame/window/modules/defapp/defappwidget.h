// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/defapp/defappworker.h"
#include "widgets/utils.h"
#include "window/insertplugin.h"

#include <dtkwidget_global.h>

#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QModelIndex;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace defapp {

class DefaultAppsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultAppsWidget(QWidget* parent = nullptr);
    virtual ~DefaultAppsWidget() override;
    void showDefaultWidget();

Q_SIGNALS:
    void requestCategoryClicked(dcc::defapp::DefAppWorker::DefaultAppsCategory category);
    void requestUpdateSecondMenu(bool);

public Q_SLOTS:
    void onCategoryClicked(const QModelIndex &index);
    void setCurrentIndex(int row);
    dcc::widgets::MultiSelectListView *getCatView() const { return m_defAppCatView; }

private:
    bool configContent(const QString & configName);

private:
    QList<ListSubItem> m_itemList;
    dcc::widgets::MultiSelectListView *m_defAppCatView;
    QStandardItemModel *m_itemModel;
    QVBoxLayout *m_centralLayout;
    QModelIndex m_lastIndex;
};
}
}
