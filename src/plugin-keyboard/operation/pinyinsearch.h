// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PINYINSEARCH_H
#define PINYINSEARCH_H

#include <QList>
#include <QStringList>

namespace dccV25 {

struct PinyinSearchIndex
{
    QList<QStringList> syllables;
    QList<QStringList> initials;
};

struct PinyinSearchQuery
{
    QString pattern;
    QList<int> prefixTable;
};

PinyinSearchIndex buildPinyinSearchIndex(const QString &text);
PinyinSearchQuery buildPinyinSearchQuery(const QString &query);
bool matchesPinyin(const PinyinSearchIndex &index, const PinyinSearchQuery &query);
bool matchesPinyinInitials(const PinyinSearchIndex &index, const PinyinSearchQuery &query);
bool matchesPinyinSearch(const PinyinSearchIndex &index, const PinyinSearchQuery &query);

}

#endif // PINYINSEARCH_H
