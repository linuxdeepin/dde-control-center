#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <libdui/libdui_global.h>

#include "private/searchdbus.h"
#include "listwidget.h"

DUI_USE_NAMESPACE

class SearchList;
class SearchItem
{
public:
    virtual ~SearchItem(){}
    virtual QStringList keyWords() const{return QStringList();}
    virtual void setData(const QVariant &){}
    virtual QVariant getData(){return QVariant();}
    virtual QWidget *widget() const = 0;

    SearchList *listWidget() const;
    virtual void setListWidget(SearchList* list);

protected:
    QPointer<SearchList> m_list;
};

class SearchList : public ListWidget
{
    Q_OBJECT

    Q_PROPERTY(int checkedItem READ checkedItem WRITE setCheckedItem NOTIFY checkedItemChanged)
    Q_PROPERTY(QString keyWord READ keyWord WRITE setKeyWord NOTIFY keyWordChanged)

public:
    explicit SearchList(QWidget *parent = 0);
    ~SearchList();

public slots:
    int addItem(SearchItem *data);
    void addItems(const QList<SearchItem*> &datas);
    void insertItem(int index, SearchItem *data);
    void insertItems(int index, const QList<SearchItem*> &datas);
    void setItemData(int index, const QVariant &data);
    void clear();
    void removeItem(int index);
    void beginSearch();
    void endSearch();
    void setKeyWord(const QString &keyWord);
    void setCheckedItem(int checkedItem);
    void updateKeyWords();

public:
    QVariant getItemData(int index) const;
    SearchItem *getItem(int index) const;
    int indexOf(const SearchItem *item) const;
    bool isSearching() const;
    int checkedItem() const;
    QString keyWord() const;

signals:
    void itemDataChanged(int index, const QVariant &data);
    void checkedItemChanged(int index);
    void keyWordChanged(QString keyWord);

private:
    QList<SearchItem*> m_itemList;
    SearchDbus *m_dbus;
    QStringList m_keyWords;//all item keyword list
    QString m_dbusKey;
    bool m_searching;
    QString m_keyWord;//current search keyword
    QStringList m_dbusKeyWords;//dbus return a list of keywords based on search keywords
};

#endif // SEARCHLIST_H
