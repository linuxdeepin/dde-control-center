// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pinyinsearch.h"

#include <DPinyin>

#include <QSet>

#include <utility>

namespace dccV25 {
namespace {

QStringList orderedUnique(const QStringList &values)
{
    QSet<QString> seen;
    QStringList result;
    seen.reserve(values.size());
    result.reserve(values.size());

    for (const QString &value : values) {
        if (seen.contains(value))
            continue;

        seen.insert(value);
        result.append(value);
    }

    return result;
}

bool matchesReadings(const QList<QStringList> &readings, const PinyinSearchQuery &query)
{
    if (query.pattern.isEmpty())
        return true;

    QList<bool> states(query.pattern.size(), false);
    states[0] = true;

    for (const QStringList &choices : readings) {
        QList<bool> nextStates(query.pattern.size(), false);
        for (qsizetype state = 0; state < states.size(); ++state) {
            if (!states.at(state))
                continue;

            for (const QString &choice : choices) {
                qsizetype matched = state;
                for (const QChar character : choice.toCaseFolded()) {
                    while (matched > 0 && character != query.pattern.at(matched))
                        matched = query.prefixTable.at(matched - 1);
                    if (character == query.pattern.at(matched))
                        ++matched;
                    if (matched == query.pattern.size())
                        return true;
                }
                nextStates[matched] = true;
            }
        }
        states = std::move(nextStates);
    }

    return false;
}

}

PinyinSearchIndex buildPinyinSearchIndex(const QString &text)
{
    DCORE_USE_NAMESPACE

    QList<QStringList> spellings;
    QList<QStringList> initials;
    spellings.reserve(text.size());
    initials.reserve(text.size());

    for (const QChar character : text) {
        QStringList choices = orderedUnique(pinyin(QString(character), TS_NoneTone));
        if (choices.isEmpty())
            choices.append(character);
        spellings.append(choices);

        for (QString &choice : choices)
            choice = choice.left(1);
        initials.append(orderedUnique(choices));
    }

    return {spellings, initials};
}

PinyinSearchQuery buildPinyinSearchQuery(const QString &query)
{
    const QString pattern = query.toCaseFolded();
    QList<int> table(pattern.size(), 0);
    for (qsizetype i = 1, matched = 0; i < pattern.size(); ++i) {
        while (matched > 0 && pattern.at(i) != pattern.at(matched))
            matched = table.at(matched - 1);
        if (pattern.at(i) == pattern.at(matched))
            ++matched;
        table[i] = matched;
    }

    return {pattern, table};
}

bool matchesPinyin(const PinyinSearchIndex &index, const PinyinSearchQuery &query)
{
    return matchesReadings(index.syllables, query);
}

bool matchesPinyinInitials(const PinyinSearchIndex &index, const PinyinSearchQuery &query)
{
    return matchesReadings(index.initials, query);
}

bool matchesPinyinSearch(const PinyinSearchIndex &index, const PinyinSearchQuery &query)
{
    return matchesPinyin(index, query) || matchesPinyinInitials(index, query);
}

}
