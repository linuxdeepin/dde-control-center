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
    DScrollArea(parent),
    m_itemWidth(-1),
    m_itemHeight(-1),
    m_layout(new QVBoxLayout),
    m_dbus(new SearchDbus(this)),
    m_searching(false),
    m_checkedItem(-1),
    m_checkable(false),
    m_mainWidget(new QWidget),
    m_visibleCount(0)
{
    D_THEME_INIT_WIDGET(SearchList);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);

    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addStretch(1);

    m_mainWidget->setObjectName("MainWidget");
    m_mainWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_mainWidget->setLayout(m_layout);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignHCenter);
    setWidget(m_mainWidget);
    setMaximumHeight(0);
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
    if(m_itemWidth>0)
        w->setFixedWidth(m_itemWidth);
    if(m_itemHeight>0){
        w->setFixedHeight(m_itemHeight);
        if(m_searching){
            updateKeyWords();
            QString key = m_dbus->NewSearchWithStrList(data->keyWords());
            QStringList tmp_list = m_dbus->SearchString(m_keyWord, key);

            if(isIntersect(tmp_list, data->keyWords()))
                setVisibleCount(m_visibleCount+1);
            else
                w->hide();
        }else{
            setVisibleCount(m_visibleCount+1);
        }
        m_mainWidget->setFixedHeight( m_visibleCount * (m_layout->spacing() + m_itemHeight) - m_layout->spacing());

        if(verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff){
            setFixedHeight(m_mainWidget->height());
        }else{
            setMaximumHeight(m_mainWidget->height());
        }
    }

    m_layout->insertWidget(index, w, 0, Qt::AlignCenter);
    if(m_checkable)
        w->installEventFilter(this);

    data->setListWidget(this);

    emit countChanged();
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

void SearchList::setItemSize(int w, int h)
{
    m_itemWidth = w;
    m_itemHeight = h;

    m_mainWidget->setFixedWidth(w);
}

void SearchList::clear()
{
    m_keyWords.clear();

    for(int i=0;i<count();++i){
        m_layout->removeItem(m_layout->takeAt(i));
        m_itemList[i]->widget()->removeEventFilter(this);
        delete m_itemList[i];
    }

    m_itemList.clear();

    m_mainWidget->setFixedHeight(0);
    setMaximumHeight(0);
    setCheckedItem(-1);

    setVisibleCount(0);

    emit countChanged();
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

    if(item){
        QWidget *w = item->widget();
        w->hide();
        if(w){
            w->removeEventFilter(this);

            if(m_searching){
                if(isIntersect(m_dbusKeyWords, item->keyWords())){
                    setVisibleCount(m_visibleCount-1);
                }
            }else{
                setVisibleCount(m_visibleCount-1);
            }
        }

        delete item;
    }

    if(index == m_checkedItem){
        setCheckedItem(-1);
    }

    if(m_itemHeight>0){
        m_mainWidget->setFixedHeight( m_visibleCount * (m_layout->spacing() + m_itemHeight) - m_layout->spacing());
        if(verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff){
            setFixedHeight(m_mainWidget->height());
        }else{
            setMaximumHeight(m_mainWidget->height());
        }

        update();
    }

    emit countChanged();
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
        m_searching = false;

        for(int i = 0; i<count(); ++i){
            showItem(i);
        }
    }
}

void SearchList::setKeyWord(const QString &keyWord)
{
    if(!m_searching || m_keyWord == keyWord)
        return;

    m_keyWord = keyWord;

    m_dbusKeyWords = m_dbus->SearchString(keyWord, m_dbusKey);

    for (int i = 0; i<count(); ++i) {
        if(keyWord.isEmpty()
                || isIntersect(m_dbusKeyWords, m_itemList[i]->keyWords())){
            showItem(i);
        }else{
            hideItem(i);
        }
    }

    emit keyWordChanged(keyWord);
}

void SearchList::showItem(int index)
{
    QWidget *w = getItem(index)->widget();

    if(w&&!w->isVisible()){
        w->show();
        setVisibleCount(m_visibleCount+1);

        if(m_itemHeight>0){
            m_mainWidget->setFixedHeight( m_visibleCount * (m_layout->spacing() + m_itemHeight) - m_layout->spacing());
            if(verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff){
                setFixedHeight(m_mainWidget->height());
            }else{
                setMaximumHeight(m_mainWidget->height());
            }
        }
    }
}

void SearchList::hideItem(int index)
{
    QWidget *w = getItem(index)->widget();

    if(w&&w->isVisible()){
        w->hide();
        setVisibleCount(m_visibleCount-1);

        if(m_itemHeight>0){
            m_mainWidget->setFixedHeight( m_visibleCount * (m_layout->spacing() + m_itemHeight) - m_layout->spacing());
            if(verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff){
                setFixedHeight(m_mainWidget->height());
            }else{
                setMaximumHeight(m_mainWidget->height());
            }
        }
    }
}

void SearchList::setCheckedItem(int checkedItem)
{
    if (m_checkedItem == checkedItem)
        return;

    if(m_checkable){
        if(m_checkedItem>=0&&m_checkedItem<count()){
            QWidget *w = getItem(m_checkedItem)->widget();
            if(w){
                w->setProperty("checked", false);
            }
        }

        if(checkedItem>=0&&checkedItem<count()){
            QWidget *w = getItem(checkedItem)->widget();
            if(w){
                w->setProperty("checked", true);
            }
        }
    }

    m_checkedItem = checkedItem;
    emit checkedItemChanged(checkedItem);
}

void SearchList::setCheckable(bool checkable)
{
    if (m_checkable == checkable)
        return;

    m_checkable = checkable;

    if(checkable){
        foreach (SearchItem *item, m_itemList) {
            QWidget *w = item->widget();
            if(w){
                w->installEventFilter(this);
            }
        }
    }else{
        foreach (SearchItem *item, m_itemList) {
            QWidget *w = item->widget();
            if(w){
                w->removeEventFilter(this);
            }
        }
    }

    emit checkableChanged(checkable);
}

void SearchList::updateKeyWords()
{
    m_dbusKey = m_dbus->NewSearchWithStrList(m_keyWords);
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

int SearchList::checkedItem() const
{
    return m_checkedItem;
}

bool SearchList::checkable() const
{
    return m_checkable;
}

int SearchList::visibleCount() const
{
    return m_visibleCount;
}

QString SearchList::keyWord() const
{
    return m_keyWord;
}

bool SearchList::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() != QEvent::MouseButtonRelease)
        return false;

    QWidget *w = qobject_cast<QWidget*>(obj);

    if(w){
        int index = indexOf(w);
        if(index>=0){
            setCheckedItem(index);
        }
    }

    return false;
}

void SearchList::setVisibleCount(int count)
{
    if(m_visibleCount == count)
        return;

    m_visibleCount = count;

    emit visibleCountChanged(count);
}

SearchList *SearchItem::listWidget() const
{
    return m_list;
}

void SearchItem::setListWidget(SearchList *list)
{
    m_list = list;
}
