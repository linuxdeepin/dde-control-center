#include "keyboardmodel.h"

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

void KeyboardModel::setLayout(const QString &value)
{
    if(m_layout == value)
        return ;

    m_layout = m_layouts.value(value);
    emit curLayout(value);
}

QString KeyboardModel::curLayout() const
{
    return m_layout;
}

void KeyboardModel::setLang(const QString &value)
{
    if(m_lang == value)
        return;

    m_lang = value;
    emit curLang(value);
}

void KeyboardModel::addUserLayout(const QString &value)
{
    m_userLayout.append(value);
}

void KeyboardModel::delUserLayout(const QString &value)
{
    m_userLayout.removeOne(value);
}

QString KeyboardModel::curLang() const
{
    return m_lang;
}

void KeyboardModel::setUserLayout(const QStringList &list)
{
    m_userLayout = list;
}

QStringList KeyboardModel::userLayout() const
{
    return m_userLayout;
}
