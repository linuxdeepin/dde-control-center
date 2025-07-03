//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "metadata.h"
#include <QDebug>

using namespace dccV25;

MetaData::MetaData(const QString &text, bool section)
    : m_text(text)
    , m_pinyin(QString())
    , m_section(section)
    , m_selected(false)
{

}

void MetaData::setPinyin(const QString &py)
{
    m_pinyin = py;
}

QString MetaData::pinyin() const
{
    return m_pinyin == QString() ? m_text : m_pinyin;
}

void MetaData::setText(const QString &text)
{
    m_text = text;
}

QString MetaData::text() const
{
    return m_text;
}

void MetaData::setKey(const QString &key)
{
    m_key = key;
}

QString MetaData::key() const
{
    return m_key;
}

void MetaData::setSection(bool section)
{
    m_section = section;
}

bool MetaData::section() const
{
    return m_section;
}

void MetaData::setSelected(bool selected)
{
    m_selected = selected;
}

bool MetaData::selected() const
{
    return m_selected;
}

bool MetaData::operator ==(const MetaData &md) const
{
    return m_text == md.m_text;
}

bool MetaData::operator >(const MetaData &md) const
{
    int x = QString::compare(m_pinyin, md.m_pinyin, Qt::CaseInsensitive);
    return x > 0;
}
