#include "keyboardmodel.h"

namespace dcc {
namespace keyboard{
KeyboardModel::KeyboardModel(QObject *parent)
    : QObject(parent)
{
}

void KeyboardModel::setLayoutLists(QMap<QString, QString> lists)
{
    m_layouts = lists;
}

QString KeyboardModel::layoutByValue(const QString &value)
{
    return m_layouts.value(value);
}

QString KeyboardModel::langByKey(const QString &key) const
{
    QList<MetaData>::const_iterator it = m_langs.begin();
    for(; it != m_langs.end(); ++it)
    {
        if((*it).key() == key)
        {
            return (*it).text();
        }
    }

    return QString();
}

void KeyboardModel::setLayout(const QString &key)
{
    QString value = m_layouts.value(key);
    if(m_layout == value)
        return ;

    m_layout = value;
    emit requestCurLayout(m_layout);
}

QString KeyboardModel::curLayout() const
{
    return m_layout;
}

void KeyboardModel::setLang(const QString &value)
{
    if(m_lang == value || value.isEmpty())
        return;

    m_lang = value;
    emit requestCurLang(m_lang);
}

void KeyboardModel::addUserLayout(const QString &value)
{
    m_userLayout.append(value);
}

void KeyboardModel::delUserLayout(const QString &value)
{
    m_userLayout.removeOne(value);
}

void KeyboardModel::setLocaleList(const QList<MetaData> &langs)
{
    m_langs = langs;
}

void KeyboardModel::setCapsLock(bool value)
{
    m_capsLock = value;

    emit requestCapsLockChanged(value);
}

void KeyboardModel::setAllShortcut(const QMap<QStringList,int> &map)
{
    m_shortcutMap = map;
}

QString KeyboardModel::curLang() const
{
    return langByKey(m_lang);
}

void KeyboardModel::setUserLayout(const QStringList &list)
{
    m_userLayout = list;
}

QStringList KeyboardModel::userLayout() const
{
    return m_userLayout;
}

QList<MetaData> KeyboardModel::langLists() const
{
    return m_langs;
}

bool KeyboardModel::capsLock() const
{
    return m_capsLock;
}

QMap<QStringList, int> KeyboardModel::allShortcut() const
{
    return m_shortcutMap;
}

}
}
