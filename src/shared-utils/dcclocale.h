//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QString>
#include <QStringList>
#include <QTranslator>

class DCCLocale
{
public:
    //! Get the dialect names of the given \a languageCodes in current system locale.
    static QStringList dialectNames(const QStringList &localeCodes);
    static QPair<QString, QString> languageAndRegionName(const QString &localeName);

    //! Strip timezone tokens from a Qt time format string while preserving
    //! locale-specific text separators and quoted literals.
    //!
    //! Handles half-width/full-width parentheses and square-bracket wrapped
    //! timezone tokens (e.g. "(tttt)", "（tttt）", "[tttt]"), as well as bare
    //! tokens with their preceding separators (e.g. "tttt HH:mm:ss",
    //! "H:mm:ss, tttt"). Quoted literals such as bg_BG "'ч'." and fr_CA
    //! "'h'/'min'/'s'" are left untouched; a trailing dangling separator
    //! (space/comma/semicolon, but not a period) is removed for cleanup.
    static QString stripTimezoneFromTimeFormat(const QString &timeFormat);
};