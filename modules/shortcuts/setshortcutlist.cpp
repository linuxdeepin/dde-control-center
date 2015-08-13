#include <QDebug>
#include <QMouseEvent>

#include <libdui/dthememanager.h>
#include <libdui/dconstants.h>

#include "setshortcutlist.h"

ShortcutWidget::ShortcutWidget(int id, const QString &title, const QString &shortcut, QWidget *parent):
    QFrame(parent),
    m_layout(new QHBoxLayout),
    m_id(id),
    m_title(new QLabel(title)),
    m_shortcut(new DShortcutEdit),
    m_animation(new QPropertyAnimation(this))
{
    m_title->setObjectName("ShortcutTitle");
    m_shortcut->setObjectName("ShortcutValue");
    setFocusPolicy(Qt::ClickFocus);

    D_THEME_INIT_WIDGET(ShortcutWidget);

    connect(m_shortcut, &DShortcutEdit::shortcutKeysFinished, [=](const QString &str){
        emit shortcutChanged(str);
        setShortcut(str);
    });

    m_shortcut->setShortcutKey(shortcut);

    m_layout->setMargin(0);
    m_layout->addSpacing(HEADER_LEFT_MARGIN);
    m_layout->addWidget(m_title, 0, Qt::AlignVCenter|Qt::AlignLeft);
    m_layout->addWidget(m_shortcut, 0, Qt::AlignVCenter|Qt::AlignRight);
    m_layout->addSpacing(HEADER_RIGHT_MARGIN);
    setLayout(m_layout);

    m_me = this;

    QString icon_path = "modules/icons/";
    icon_path = icon_path.append(DThemeManager::instance()->theme().append("/delete_multi_"));
    m_removeButton = new DImageButton(
                icon_path+"normal.png", icon_path+"hover.png", icon_path+"press.png", this);
    m_removeButton->setFixedSize(IMAGE_BUTTON_WIDTH, 30);
    m_removeButton->move(-m_removeButton->width(), m_removeButton->y());
    connect(m_removeButton, &DImageButton::clicked, [=]{
        emit removeShortcut();
    });
    m_animation->setTargetObject(m_removeButton);
    m_animation->setPropertyName("geometry");
    connect(m_animation, &QPropertyAnimation::valueChanged, [=](const QVariant &value){
        QRect rect = m_layout->geometry();
        rect.setX(value.toRect().right()/1.5);
        m_layout->setGeometry(rect);
    });
}

void ShortcutWidget::setId(int id)
{
    m_id = id;
}

void ShortcutWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void ShortcutWidget::setShortcut(const QString &shortcut)
{
    if (m_shortcuText == shortcut)
        return;

    m_shortcut->setShortcutKey(shortcut);
    m_shortcuText = shortcut;
    emit shortcutChanged(shortcut);
}

void ShortcutWidget::showRemoveButton() const
{
    m_animation->setStartValue(QRect(-IMAGE_BUTTON_WIDTH, 0, IMAGE_BUTTON_WIDTH, 30));
    m_animation->setEndValue(QRect(HEADER_LEFT_MARGIN, 0, IMAGE_BUTTON_WIDTH, 30));
    m_animation->start();
    m_shortcut->setDisabled(false);
    m_shortcut->setEnabled(false);
}

void ShortcutWidget::hideRemoveButton() const
{
    m_animation->setEndValue(QRect(-IMAGE_BUTTON_WIDTH, 0, IMAGE_BUTTON_WIDTH, 30));
    m_animation->setStartValue(QRect(HEADER_LEFT_MARGIN, 0, IMAGE_BUTTON_WIDTH, 30));
    m_animation->start();
}

int ShortcutWidget::id() const
{
    return m_id;
}

QString ShortcutWidget::title() const
{
    return m_title->text();
}

QString ShortcutWidget::shortcut() const
{
    return m_shortcuText;
}

QStringList ShortcutWidget::keyWords() const
{
    return QStringList()<<m_title->text()<<m_shortcuText;
}

void ShortcutWidget::setData(const QVariant &datas)
{
    QVariantList list = datas.toList();
    if(list.count()==3&&list[0].type()==QVariant::Int
            &&list[1].type()==QVariant::String&&list[2].type()==QVariant::String){
        setId(list[0].toInt());
        m_title->setText(list[1].toString());
        m_shortcut->setShortcutKey(list[2].toString());
    }
}

QVariant ShortcutWidget::getData()
{
    return QVariantList()<<m_id<<m_title->text()<<m_shortcuText;
}

QWidget *ShortcutWidget::widget() const
{
    return m_me;
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

int SetShortcutList::addItem(int id, const QString &title, const QString &shortcut)
{
    insertItem(count(), id, title, shortcut);
    return count()-1;
}

void SetShortcutList::addItems(QList<int> ids, const QStringList &titles, const QStringList &shortcuts)
{
    insertItems(count(), ids, titles, shortcuts);
}

void SetShortcutList::insertItem(int index, int id, const QString &title, const QString &shortcut)
{
    ShortcutWidget *w = new ShortcutWidget(id, title, shortcut);
    w->setFixedSize(m_itemWidth, m_itemHeight);
    m_layout->insertWidget(index, w, 0, Qt::AlignHCenter);
    setFixedHeight(count()*(m_layout->spacing()+m_itemHeight)-m_layout->spacing());
    connect(w, SIGNAL(shortcutChanged(QString)), SLOT(shortcutChanged(QString)));
}

void SetShortcutList::insertItems(int index, QList<int> ids, const QStringList &titles, const QStringList &shortcuts)
{
    for(int i=0;i<titles.count();++i){
        insertItem(index+i, ids[i], titles[i], i<shortcuts.count()?shortcuts[i]:"");
    }
}

void SetShortcutList::setItemId(int index, int id)
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        w->setId(id);
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

void SetShortcutList::clear()
{
    int count = this->count();
    for(int i=0;i<count;++i){
        QWidget *w = getItemWidget(0);
        m_layout->removeWidget(w);
        w->deleteLater();
    }

    setFixedHeight(1);
    setFixedHeight(0);
}

void SetShortcutList::removeItem(int index)
{
    QWidget *w = getItemWidget(index);
    if(w){
        m_layout->removeWidget(w);
        w->deleteLater();
        setFixedHeight(height()-m_layout->spacing()-m_itemHeight);
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

QString SetShortcutList::getItemShortcut(int index) const
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        return w->shortcut();
    }

    return "";
}

int SetShortcutList::getItemId(int index) const
{
    ShortcutWidget *w = getItemWidget(index);
    if(w){
        return w->id();
    }

    return -1;
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

int SetShortcutList::indexOfById(int id) const
{
    for(int i=0;i<count();++i){
        ShortcutWidget *w = getItemWidget(i);
        if(w&&w->id() == id){
            return i;
        }
    }

    return -1;
}

ShortcutWidget *SetShortcutList::getItemWidget(int index) const
{
    if(index>=0&&index<count())
        return qobject_cast<ShortcutWidget*>(m_layout->itemAt(index)->widget());

    return NULL;
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
