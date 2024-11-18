// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "appslistmodel.h"
#include "appssourcemodel.h"

using namespace DCC_NAMESPACE;

AppsListModel::AppsListModel(QObject *parent)
    : QSortFilterProxyModel{ parent }
{
}

bool AppsListModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    QString l_transliterated = sourceModel()->data(source_left, TransliteratedRole).toString();
    QString r_transliterated = sourceModel()->data(source_right, TransliteratedRole).toString();
    QChar l_start = l_transliterated.isEmpty() ? QChar() : l_transliterated.constData()[0].toUpper();
    QChar r_start = r_transliterated.isEmpty() ? QChar() : r_transliterated.constData()[0].toUpper();
    if (l_start != r_start) {
        return l_start < r_start;
    } else {
        QString l_display = source_left.model()->data(source_left, Qt::DisplayRole).toString();
        QString r_display = source_right.model()->data(source_right, Qt::DisplayRole).toString();
        QChar ld_start = l_display.isEmpty() ? QChar() : l_display.constData()[0].toUpper();
        QChar rd_start = r_display.isEmpty() ? QChar() : r_display.constData()[0].toUpper();
        if ((l_start == ld_start && ld_start == rd_start) || (l_start != ld_start && l_start != rd_start)) {
            // display name both start with ascii letter, or both NOT start with ascii letter
            // use their transliterated form for sorting
            if (!l_start.isNull() && l_transliterated.constData()[0] != r_transliterated.constData()[0]) {
                // Since in ascii table, `A` is lower than `a`, we specially check to ensure `a` is lower here.
                return l_transliterated.constData()[0].isLower();
            }
            return l_transliterated < r_transliterated;
        } else {
            // one of them are ascii letter and another of them is non-ascii letter.
            // the ascii one should be display on the front
            return l_start == ld_start;
        }
    }
}
