#include "keyboardcontrol.h"
#include "keyitem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

KeyboardControl::KeyboardControl(QFrame *parent)
    : QFrame(parent),
      m_modifiers(false),
      m_keycount(0)
{
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::StrongFocus);
    setFixedHeight(180);

    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusWidget(QWidget*,QWidget*)));
}

KeyboardControl::~KeyboardControl()
{
    KeyItem::deleteItems();
}

void KeyboardControl::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::Iterator it = list.begin();
    for(; it != list.end(); it++)
    {
        (*it)->paint(&painter, rect());
    }
}

void KeyboardControl::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers() != Qt::NoModifier)
    {
        m_modifiers = true;
    }

    if(m_modifiers)
    {
        QList<KeyItem*> list = KeyItem::keyboards();
        QList<KeyItem*>::Iterator it = list.begin();
        for(; it != list.end(); it++)
        {
            KeyItem* info = *it;
            if(info->keycode() == e->nativeScanCode())
            {
                info->setPress(true);
                if(!m_stack.contains(info) && info->modifies())
                    append(info);
                else
                    m_last = info;
            }
        }
    }

    m_keycount++;
    update();
}

void KeyboardControl::keyReleaseEvent(QKeyEvent *e)
{
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::Iterator it = list.begin();
    for(; it != list.end(); it++)
    {
        if((*it)->keycode() == e->nativeScanCode())
            (*it)->setPress(false);
    }

    m_keycount--;
    if(m_keycount == 0 && m_stack.count() != 0)
    {
        m_modifiers = false;
        if(m_last)
        {
            m_stack.append(m_last);
        }

        QString str;
        for(int i = 0; i<m_stack.count(); i++)
            str += m_stack.at(i)->mainKey() + ((i == m_stack.count()-1) ? "" : "-");

        emit shortcutChanged(str);
        qDebug()<<Q_FUNC_INFO<<str;
        m_stack.clear();
        m_last = NULL;
    }

    update();
}

void KeyboardControl::append(KeyItem *item)
{
    QStringList list;
    list<<"Ctrl"<<"Alt"<<"Super"<<"Shift"<<"Fn";

    if(m_stack.count() == 0)
    {
        m_stack.append(item);
        return;
    }

    int index = list.indexOf(item->mainKey());

    int record = -1;
    for(int i = 0; i< m_stack.count(); i++)
    {
        int index_1 = list.indexOf(m_stack.at(i)->mainKey());
        if(index_1 < 0)
            continue;

        if(index < index_1)
        {
            record = i;
        }
    }

    if(record != -1)
        m_stack.insert(record,item);
    else
        m_stack.append(item);
}

void KeyboardControl::onFocusWidget(QWidget *old, QWidget *now)
{
    Q_UNUSED(old);
    Q_UNUSED(now);

    m_modifiers = false;
    m_keycount = 0;
    m_stack.clear();
    m_last = NULL;
}

