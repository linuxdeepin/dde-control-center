/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef INDEXMODEL_H
#define INDEXMODEL_H

#include <QString>
#include <QAbstractListModel>
#include <QItemDelegate>
#include <QFrame>

namespace dcc {
namespace keyboard{

class MetaData
{
public:
    MetaData(const QString& text = QString(), bool section = false);

    void setPinyin(const QString& py);
    QString pinyin() const;

    void setText(const QString& text);
    QString text() const;

    void setKey(const QString& key);
    QString key() const;

    void setSection(bool section);
    bool section() const;

    void setSelected(bool selected);
    bool selected() const;

    bool operator ==(const MetaData& md) const;
    bool operator >(const MetaData& md) const;
private:
    QString m_key;
    QString m_text;
    QString m_pinyin;
    bool m_section;
    bool m_selected;
    friend QDebug& operator<<(QDebug dbg, const MetaData& md);
};

QDebug& operator<<(QDebug dbg, const MetaData& md);

class IndexModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IndexModel(QObject* parent = 0);

    void setMetaData(const QList<MetaData>& datas);
    QList<MetaData> metaData() const;
    int  indexOf(const MetaData& md);

    void setLetters(QList<QString> letters);
    QList<QString> letters() const;

//    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QList<MetaData> m_datas;
    QList<QString> m_letters;
};

}
}
Q_DECLARE_METATYPE(dcc::keyboard::MetaData)

#endif // INDEXMODEL_H
