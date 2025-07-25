//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
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
};