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
