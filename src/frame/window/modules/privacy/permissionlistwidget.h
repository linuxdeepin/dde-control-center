// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef PERMISSIONLISTWIDGET_H
#define PERMISSIONLISTWIDGET_H

#include "modules/privacy/applicationitem.h"

#include <QWidget>

//QT_BEGIN_NAMESPACE
class QStandardItemModel;
//QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class MultiSelectListView;
}
}
class PermissionListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PermissionListWidget(const QList<PrivacyPage> &privactPages, QWidget *parent = nullptr);

Q_SIGNALS:
    void requestShowPage(int id);

public Q_SLOTS:
    void onCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
    void setCurrent(int id);

private:
    dcc::widgets::MultiSelectListView *m_lvnmpages;
};

#endif // PERMISSIONLISTWIDGET_H
