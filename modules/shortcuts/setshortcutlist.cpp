#include "setshortcutlist.h"

SetShortcutList::SetShortcutList(QWidget *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout)
{

}

int SetShortcutList::addItem(const QString &title, const QString &shortcutName)
{
    insertItem(0, title, shortcutName);
    return count()-1;
}

void SetShortcutList::addItems(const QStringList &titles, const QStringList &shortcutNames)
{
    insertItems(0, titles, shortcutNames);
}

void SetShortcutList::insertItem(int index, const QString &title, const QString &shortcutName)
{
    ShortcutWidget *w = new ShortcutWidget(title, shortcutName);
    m_layout->insertWidget(index, w, 0, Qt::AlignVCenter);
    connect(w, SIGNAL(shortcutNameChanged(QString)), SLOT(shortcutNameChanged(QString)));
}

void SetShortcutList::insertItems(int index, const QStringList &titles, const QStringList &shortcutNames)
{
    for(int i=0;i<titles.count();++i){
        insertItem(index+i, titles[i], shortcutNames[i]);
    }
}

void SetShortcutList::setItemTitle(int index, const QString &title)
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        w->setTitle(title);
    }
}

void SetShortcutList::setItemShortcutName(int index, const QString &name)
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        w->setShortcutName(name);
    }
}

int SetShortcutList::count() const
{
    return m_layout->count();
}

QString SetShortcutList::getItemTitle(int index) const
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        return w->title();
    }

    return "";
}

QString SetShortcutList::getItemShortcutName(int index) const
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        return w->shortcutName();
    }

    return "";
}

int SetShortcutList::indexOfByTitle(const QString &title) const
{
    for(int i=0;i<count();++i){
        ShortcutWidget *w = getItemWidget(i);
        if(w&&w->title() == title){
            return i;
        }
    }

    return -1;
}

ShortcutWidget *SetShortcutList::getItemWidget(int index) const
{
    return qobject_cast<ShortcutWidget*>(m_layout->takeAt(index)->widget());
}

void SetShortcutList::shortcutNameChanged(const QString &name)
{
    ShortcutWidget *w = qobject_cast<ShortcutWidget*>(sender());

    if(w){
        int index = m_layout->indexOf(w);
        if(index>=0&&index<count())
            emit shortcutNameChanged(m_layout->indexOf(w), name);
        emit shortcutNameChanged(w->title(), name);
    }
}



ShortcutWidget::ShortcutWidget(const QString &title, const QString &shortcutName, QWidget *parent):
    QFrame(parent),
    m_shortcutName(shortcutName),
    m_title(title)
{

}

void ShortcutWidget::setTitle(const QString &title)
{
    m_title = title;
}

void ShortcutWidget::setShortcutName(const QString &shortcutName)
{
    if (m_shortcutName == shortcutName)
        return;

    m_shortcutName = shortcutName;
    emit shortcutNameChanged(shortcutName);
}

QString ShortcutWidget::title() const
{
    return m_title;
}

QString ShortcutWidget::shortcutName() const
{
    return m_shortcutName;
}
