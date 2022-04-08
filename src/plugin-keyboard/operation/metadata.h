/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "interface/namespace.h"
#include <QObject>

namespace DCC_NAMESPACE {

class MetaData
{
public:
    MetaData(const QString &text = QString(), bool section = false);

    void setPinyin(const QString &py);
    QString pinyin() const;

    void setText(const QString &text);
    QString text() const;

    void setKey(const QString &key);
    QString key() const;

    void setSection(bool section);
    bool section() const;

    void setSelected(bool selected);
    bool selected() const;

    bool operator ==(const MetaData &md) const;
    bool operator >(const MetaData &md) const;
private:
    QString m_key;
    QString m_text;
    QString m_pinyin;
    bool m_section;
    bool m_selected;
    friend QDebug &operator<<(QDebug dbg, const MetaData &md);
};

QDebug &operator<<(QDebug dbg, const MetaData &md);

}
Q_DECLARE_METATYPE(DCC_NAMESPACE::MetaData)
