// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef INDEXMODEL_H
#define INDEXMODEL_H

#include <DListView>

#include <QString>
#include <QStandardItemModel>
#include <QItemDelegate>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard {

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

class IndexModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit IndexModel(QObject *parent = nullptr);

    void setMetaData(const QList<MetaData> &datas);
    QList<MetaData> metaData() const;
    int  indexOf(const MetaData &md);

    void setLetters(QList<QString> &letters);
    QList<QString> letters() const;
    int getModelCount();

protected:
    int rowCount(const QModelIndex &parent) const;
private:
    QList<MetaData> m_datas;
    QList<QString> m_letters;
public:
    enum {
        KBLayoutRole = Dtk::UserRole + 1,
    };
};

}
}
Q_DECLARE_METATYPE(dcc::keyboard::MetaData)

#endif // INDEXMODEL_H
