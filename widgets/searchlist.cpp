#include "searchlist.h"

SearchList::SearchList(QWidget *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout),
    m_listWidget(new QListWidget),
    m_dbus(new SearchDbus(this)),
    m_searching(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_listWidget->setSelectionMode(QListWidget::NoSelection);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollMode(QListWidget::ScrollPerItem);
    m_listWidget->setObjectName("SearchListWidget");
    m_listWidget->setStyleSheet("QListWidget#SearchListWidget{background-color: transparent;}");

    m_layout->setMargin(0);
    m_layout->addWidget(m_listWidget);
    setLayout(m_layout);
}

int SearchList::addItem(SearchItem *data)
{
    insertItem(count(), data);

    return count()-1;
}

void SearchList::addItems(const QList<SearchItem*> &datas)
{
    insertItems(count(), datas);
}

void SearchList::insertItem(int index, SearchItem *data)
{
    if(data==NULL||data->widget()==NULL)
        return;

    QListWidgetItem *item = new QListWidgetItem(m_listWidget);
    item->setBackgroundColor(Qt::transparent);
    m_listWidget->insertItem(index, item);
    m_itemList<<data;
    m_listWidget->setItemWidget(item, const_cast<QWidget*>(data->widget()));
    m_keyWords<<data->keyWords();
}

void SearchList::insertItems(int index, const QList<SearchItem*> &datas)
{
    foreach (SearchItem *data, datas) {
        insertItem(index++, data);
    }
}

void SearchList::setItemData(int index, const QVariant &data)
{
    SearchItem *item = getItem(index);
    if(item){
        item->setData(data);
    }
}

void SearchList::setItemSize(int w, int h)
{
    m_listWidget->setGridSize(QSize(w, h));
}

void SearchList::clear()
{
    m_listWidget->clear();
    m_itemList.clear();
    m_keyWords.clear();
}

void SearchList::removeItem(int index)
{
    QListWidgetItem *tmp = m_listWidget->item(index);
    m_listWidget->removeItemWidget(tmp);
    delete tmp;
    SearchItem *item = getItem(index);
    m_itemList.removeAt(index);

    if(item){
        foreach (const QString &str, item->keyWords()) {
            m_keyWords.removeOne(str);
        }
    }
}

void SearchList::beginSearch()
{
    if(m_searching)
        return;
    m_searching = true;
    m_backupItemList = m_itemList;
    m_dbusKey = m_dbus->NewSearchWithStrList(m_keyWords);
}

void SearchList::endSearch()
{
    if(m_searching){
        m_dbusKey = "";
        m_searching = false;

        for(int i = 0; i<count(); ++i)
            m_listWidget->setRowHidden(i, false);
    }
}

bool isIntersect(const QStringList& list1, const QStringList &list2)
{
    foreach (const QString &str, list1) {
        if(list2.contains(str))
            return true;
    }

    return false;
}

void SearchList::setKeyWord(const QString &keyWord)
{
    if(m_searching&&!keyWord.isEmpty()){
        QStringList list = m_dbus->SearchString(keyWord, m_dbusKey);

        for (int i = 0; i<count(); ++i) {
            m_listWidget->setRowHidden(i, !isIntersect(list, m_itemList[i]->keyWords()));
        }
    }
}

int SearchList::count() const
{
    return m_itemList.count();
}

QVariant SearchList::getItemData(int index) const
{
    return getItem(index)->getData();
}

SearchItem *SearchList::getItem(int index) const
{
    return m_itemList[index];
}

int SearchList::indexOf(SearchItem *item) const
{
    return m_itemList.indexOf(item);
}

QListWidget *SearchList::listWidget() const
{
    return m_listWidget;
}

bool SearchList::isSearching() const
{
    return m_searching;
}

void SearchList::itemDataChanged(const QVariant &data)
{
    SearchItem *item = dynamic_cast<SearchItem*>(sender());
    if(item){
        emit itemDataChanged(indexOf(item), data);
    }
}
