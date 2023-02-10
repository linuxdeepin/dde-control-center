//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

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
