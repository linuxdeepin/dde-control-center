#include <libdui/dthememanager.h>

#include "setshortcutlist.h"

DUI_USE_NAMESPACE

ShortcutWidget::ShortcutWidget(const QString &title, const QString &shortcut, QWidget *parent):
    QFrame(parent),
    m_layout(new QHBoxLayout),
    m_title(new QLabel(title)),
    m_shortcut(new QLabel("shortcut"))
{
    m_title->setObjectName("ShortcutTitle");
    m_shortcut->setObjectName("ShortcutValue");

    D_THEME_INIT_WIDGET(ShortcutWidget);

    m_layout->setMargin(0);
    m_layout->addWidget(m_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
    m_layout->addWidget(m_shortcut, 0, Qt::AlignVCenter|Qt::AlignRight);
    setLayout(m_layout);
}

void ShortcutWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void ShortcutWidget::setShortcut(const QString &shortcut)
{
    if (m_shortcut->text() == shortcut)
        return;

    m_shortcut->setText(shortcut);
    emit shortcutChanged(shortcut);
}

QString ShortcutWidget::title() const
{
    return m_title->text();
}

QString ShortcutWidget::shortcut() const
{
    return m_shortcut->text();
}


SetShortcutList::SetShortcutList(QWidget *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout(this)),
    m_itemWidth(-1),
    m_itemHeight(-1)
{
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

int SetShortcutList::addItem(const QString &title, const QString &shortcut)
{
    insertItem(0, title, shortcut);
    return count()-1;
}

void SetShortcutList::addItems(const QStringList &titles, const QStringList &shortcuts)
{
    insertItems(0, titles, shortcuts);
}

void SetShortcutList::insertItem(int index, const QString &title, const QString &shortcut)
{
    ShortcutWidget *w = new ShortcutWidget(title, shortcut);
    w->setFixedSize(m_itemWidth, m_itemHeight);
    m_layout->insertWidget(index, w, 0, Qt::AlignCenter);
    setFixedHeight(count()*(m_itemHeight+m_layout->spacing())-m_layout->spacing());
    connect(w, SIGNAL(shortcutChanged(QString)), SLOT(shortcutChanged(QString)));
}

void SetShortcutList::insertItems(int index, const QStringList &titles, const QStringList &shortcuts)
{
    for(int i=0;i<titles.count();++i){
        insertItem(index+i, titles[i], i<shortcuts.count()?shortcuts[i]:"");
    }
}

void SetShortcutList::setItemTitle(int index, const QString &title)
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        w->setTitle(title);
    }
}

void SetShortcutList::setItemShortcut(int index, const QString &value)
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        w->setShortcut(value);
    }
}

void SetShortcutList::setItemSize(int w, int h)
{
    m_itemWidth = w;
    m_itemHeight = h;
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

QString SetShortcutList::getItemShortcut(int index) const
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        return w->shortcut();
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

void SetShortcutList::shortcutChanged(const QString &value)
{
    ShortcutWidget *w = qobject_cast<ShortcutWidget*>(sender());

    if(w){
        int index = m_layout->indexOf(w);
        if(index>=0&&index<count())
            emit shortcutChanged(m_layout->indexOf(w), value);
        emit shortcutChanged(w->title(), value);
    }
}
