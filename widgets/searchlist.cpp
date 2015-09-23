#include <QDebug>

#include <libdui/dthememanager.h>

#include "searchlist.h"

bool isIntersect(const QStringList& list1, const QStringList &list2)
{
    foreach (const QString &str, list1) {
        if(list2.contains(str))
            return true;
    }

    return false;
}

SearchList::SearchList(QWidget *parent) :
    ListWidget(Radio, parent),
    m_dbus(new SearchDbus(this)),
    m_searching(false)
{
    D_THEME_INIT_WIDGET(SearchList);
    connect(this, &SearchList::firstCheckedChanged, this, &SearchList::checkedItemChanged);
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
    if(data==NULL || data->widget()==NULL || m_itemList.contains(data))
        return;

    m_itemList.insert(index, data);
    m_keyWords << data->keyWords();

    QWidget *w = data->widget();
    ListWidget::insertWidget(index, w);

    if(m_searching){
        updateKeyWords();
        QDBusPendingReply<QString, bool> reply = m_dbus->NewSearchWithStrList(data->keyWords());
        reply.waitForFinished();
        QString key = reply.value();
        QDBusPendingReply<QStringList> search_reply = m_dbus->SearchString(m_keyWord, key);
        search_reply.waitForFinished();
        QStringList tmp_list = search_reply.value();

        if(tmp_list.isEmpty())
            hideWidget(index);
    }

    data->setListWidget(this);
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
    if(item&&item->getData()!=data){
        item->setData(data);
        emit itemDataChanged(index, data);
    }
}

void SearchList::clear()
{
    ListWidget::clear(false);

    foreach (SearchItem *item, m_itemList) {
        delete item;
    }

    m_itemList.clear();
    m_keyWords.clear();
}

void SearchList::removeItem(int index)
{
    SearchItem *item = getItem(index);
    m_itemList.removeAt(index);
    ListWidget::removeWidget(index, false);

    if(item){
        foreach (const QString &str, item->keyWords()) {
            m_keyWords.removeOne(str);
        }

        QWidget *w = item->widget();
        if(w){
            if(m_searching){
                if(isIntersect(m_dbusKeyWords, item->keyWords())){
                    setVisibleCount(visibleCount()-1);
                }
            }else{
                setVisibleCount(visibleCount()-1);
            }
        }

        delete item;
    }
}

void SearchList::beginSearch()
{
    if(m_searching)
        return;
    m_searching = true;
    updateKeyWords();
}

void SearchList::endSearch()
{
    if(m_searching){
        m_dbusKey = "";
        m_keyWord = "";
        m_searching = false;

        for(int i = 0; i<count(); ++i){
            showWidget(i);
        }
    }
}

void SearchList::setKeyWord(const QString &keyWord)
{
    if(!m_searching || m_keyWord == keyWord)
        return;

    m_keyWord = keyWord;

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbus->SearchString(keyWord, m_dbusKey), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher]{
        QDBusPendingReply<QStringList> reply = *watcher;
        m_dbusKeyWords = reply.value();
        for (int i = 0; i<count(); ++i) {
            if(m_keyWord.isEmpty()
                    || isIntersect(m_dbusKeyWords, m_itemList[i]->keyWords())){
                showWidget(i);
            }else{
                hideWidget(i);
            }
        }

        watcher->deleteLater();
    });

    emit keyWordChanged(keyWord);
}

void SearchList::setCheckedItem(int checkedItem)
{
    setChecked(checkedItem, true);
}

void SearchList::updateKeyWords()
{
    QDBusPendingReply<QString, bool> reply = m_dbus->NewSearchWithStrList(m_keyWords);
    reply.waitForFinished();
    m_dbusKey = reply;
}

QVariant SearchList::getItemData(int index) const
{
    return getItem(index)->getData();
}

SearchItem *SearchList::getItem(int index) const
{
    return m_itemList[index];
}

int SearchList::indexOf(const SearchItem *item) const
{
    return m_itemList.indexOf(const_cast<SearchItem*>(item));
}

bool SearchList::isSearching() const
{
    return m_searching;
}

int SearchList::checkedItem() const
{
    return firstChecked();
}

QString SearchList::keyWord() const
{
    return m_keyWord;
}

SearchList *SearchItem::listWidget() const
{
    return m_list;
}

void SearchItem::setListWidget(SearchList *list)
{
    m_list = list;
}
