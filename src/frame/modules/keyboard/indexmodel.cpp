// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <DStandardItem>

#include "indexmodel.h"
#include <QDBusInterface>
#include <com_deepin_daemon_inputdevice_keyboard.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard {

using DBusKeyboard = com::deepin::daemon::inputdevice::Keyboard;

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

QDebug &operator<<(QDebug dbg, const MetaData &md)
{
    dbg.nospace() << "key: " << md.key() << endl;
    dbg.nospace() << "text: " << md.text() << endl;
    return dbg.maybeSpace();
}

IndexModel::IndexModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

void IndexModel::setMetaData(const QList<MetaData> &datas)
{
    beginResetModel();
    m_datas = datas;
    for (int i = 0; i < m_datas.size(); ++i) {
        DStandardItem *item = new DStandardItem(m_datas[i].text());
        item->setData(QVariant::fromValue(m_datas[i]), KBLayoutRole);
        appendRow(item);
    }
    endResetModel();
}

QList<MetaData> IndexModel::metaData() const
{
    return m_datas;
}

int IndexModel::indexOf(const MetaData &md)
{
    int index = 0;
    QList<MetaData>::iterator it = m_datas.begin();
    for (; it != m_datas.end(); ++it) {
        if ((*it) == md && (*it).section()) {
            return index;
        }
        index++;
    }

    return -1;
}

void IndexModel::setLetters(QList<QString> &letters)
{
    m_letters = letters;
}

QList<QString> IndexModel::letters() const
{
    return m_letters;
}

int IndexModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_datas.count();
}

int IndexModel::getModelCount()
{
    return m_datas.count();
}

}
}
