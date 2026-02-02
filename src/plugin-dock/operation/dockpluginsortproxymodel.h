// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QSortFilterProxyModel>

class DockPluginSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    enum StringGroup {
        Digit = 0,
        Latin = 1,
        CJK   = 2
    };

public:
    explicit DockPluginSortProxyModel(QObject *parent = nullptr);

    DockPluginSortProxyModel::StringGroup classifyString(const QString &s) const;

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};
