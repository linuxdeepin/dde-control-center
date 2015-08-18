#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "searchlist.h"

DUI_USE_NAMESPACE

SearchList::SearchList(QWidget *parent) :
    QFrame(parent),
    m_itemWidth(-1),
    m_itemHeight(-1),
    m_layout(new QVBoxLayout),
    m_dbus(new SearchDbus(this)),
    m_searching(false)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    D_THEME_INIT_WIDGET(SearchList);

    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addStretch(1);
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

    m_itemList.insert(index, data);
    QWidget *w = data->widget();
    if(m_itemWidth>0)
        w->setMinimumWidth(m_itemWidth);
    if(m_itemHeight>0)
        w->setMinimumHeight(m_itemHeight);
    m_layout->insertWidget(index, data->widget());
    m_keyWords<<data->keyWords();
    setMinimumHeight(count()*(m_layout->spacing()+m_itemHeight)-m_layout->spacing());
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
    m_itemWidth = w;
    m_itemHeight = h;
}

void SearchList::clear()
{
    m_itemList.clear();
    m_keyWords.clear();

    setMinimumHeight(0);
}

void SearchList::removeItem(int index)
{
    SearchItem *item = getItem(index);
    m_itemList.removeAt(index);

    if(item){
        foreach (const QString &str, item->keyWords()) {
            m_keyWords.removeOne(str);
        }
    }
    m_layout->removeItem(m_layout->takeAt(index));
    if(item)
        delete item;
    setMinimumHeight(count()*(m_layout->spacing()+m_itemHeight)-m_layout->spacing());
}

void SearchList::beginSearch()
{
    if(m_searching)
        return;
    m_searching = true;
    m_dbusKey = m_dbus->NewSearchWithStrList(m_keyWords);
}

void SearchList::endSearch()
{
    if(m_searching){
        m_dbusKey = "";
        m_searching = false;

        for(int i = 0; i<count(); ++i){
            showItem(i);
        }
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
            if(isIntersect(list, m_itemList[i]->keyWords())){
                showItem(i);
            }else{
                hideItem(i);
            }
        }
    }
}

void SearchList::showItem(int index)
{
    QWidget *w = getItem(index)->widget();

    if(w&&!w->isVisible()){
        w->show();
        setMinimumHeight(minimumHeight()+m_layout->spacing()+m_itemHeight);
        m_layout->update();
    }
}

void SearchList::hideItem(int index)
{
    QWidget *w = getItem(index)->widget();
    if(w&&w->isVisible()){
        w->hide();
        setMinimumHeight(minimumHeight()-m_layout->spacing()-m_itemHeight);
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

int SearchList::indexOf(QWidget *widget) const
{
    for (int i=0;i<count();++i) {
        const SearchItem *item = m_itemList[i];
        if(item->widget() == widget){
            return i;
        }
    }

    return -1;
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
