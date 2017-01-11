#include "keyboardcontrol.h"
#include "keyitem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

namespace dcc {
namespace keyboard{
KeyboardControl::KeyboardControl(QFrame *parent)
    : SettingsItem(parent),
      m_modifiers(false),
      m_keycount(0)
{
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::StrongFocus);
    setFixedHeight(180);
}

KeyboardControl::~KeyboardControl()
{
    KeyItem::deleteItems();
}

void KeyboardControl::setConflictString(const QStringList &list)
{
    m_conflicts = list;
}

void KeyboardControl::setPress(const QString &key, bool press)
{
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        if((*it)->mainKey() ==  key && !m_conflicts.contains(key))
        {
            (*it)->setPress(press);
        }
    }
    update();
}

void KeyboardControl::setConflicts(const QString &key, bool press)
{
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        if((*it)->mainKey() ==  key)
        {
            (*it)->setPress(press);
        }
    }
    update();
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
}
}
