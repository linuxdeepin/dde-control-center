#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QDebug>

#include "private/searchdbus.h"

class SearchItem
{
public:
    virtual ~SearchItem(){}
    virtual QStringList keyWords() const = 0;
    virtual void setData(const QVariant &datas) = 0;
    virtual QVariant getData() = 0;
    virtual QWidget *widget() const = 0;
};

class SearchList : public QFrame
{
    Q_OBJECT
public:
    explicit SearchList(QWidget *parent = 0);

public slots:
    int addItem(SearchItem *data);
    void addItems(const QList<SearchItem*> &datas);
    void insertItem(int index, SearchItem *data);
    void insertItems(int index, const QList<SearchItem*> &datas);
    void setItemData(int index, const QVariant &data);
    void setItemSize(int w, int h);
    void clear();
    void removeItem(int index);
    void beginSearch();
    void endSearch();
    void setKeyWord(const QString &keyWord);

public:
    int count() const;
    QVariant getItemData(int index) const;
    SearchItem *getItem(int index) const;
    int indexOf(SearchItem *item) const;
    QListWidget *listWidget() const;
    bool isSearching() const;

signals:
    void itemDataChanged(int index, const QVariant &data);

private slots:
    void itemDataChanged(const QVariant &data);

private:
    QVBoxLayout *m_layout;
    QListWidget *m_listWidget;
    QList<SearchItem*> m_itemList;
    SearchDbus *m_dbus;
    QStringList m_keyWords;
    QString m_dbusKey;
    QList<SearchItem*> m_backupItemList;
    bool m_searching;
};

#endif // SEARCHLIST_H
