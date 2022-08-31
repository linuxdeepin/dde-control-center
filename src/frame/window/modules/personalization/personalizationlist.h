// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "window/utils.h"
#include "window/insertplugin.h"

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

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace personalization {
class PersonalizationList : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalizationList(QWidget *parent = nullptr);
    virtual ~PersonalizationList() override;
    void showDefaultWidget();

Q_SIGNALS:
    void requestShowGeneral();
    void requestShowIconTheme();
    void requestShowCursorTheme();
    void requestShowFonts();
    void requestUpdateSecondMenu(bool);

public Q_SLOTS:
    void onCategoryClicked(const QModelIndex &index);
    void setCurrentIndex(int row);

private:
    bool configContent(const QString & configName);

private:
    QList<ListSubItem> m_itemList;
    dcc::widgets::MultiSelectListView *m_categoryListView;
    QStandardItemModel *m_model;
    QVBoxLayout *m_centralLayout;
    QModelIndex m_lastIndex;
};
}
}
