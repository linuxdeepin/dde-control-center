#include "keyboardmodel.h"
#include <QDebug>

namespace dcc {
namespace keyboard{
KeyboardModel::KeyboardModel(QObject *parent)
    : QObject(parent)
{
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardModel::setLayoutLists(QMap<QString, QString> lists)
{
    m_layouts = lists;
}
#endif

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

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardModel::setLayout(const QString &key)
{
    if (key.isEmpty())
        return;

    if(m_layout == key)
        return ;

    m_layout = key;

    emit curLayoutChanged(m_layout);
}
#endif

QString KeyboardModel::curLayout() const
{
    return m_layout;
}

void KeyboardModel::setLang(const QString &value)
{
    if (m_lang != value && !value.isEmpty()) {
        m_lang = value;

        const QString &key = langByKey(value);
        if (!key.isEmpty())
            emit curLangChanged(key);
    }
}

void KeyboardModel::setLocaleList(const QList<MetaData> &langs)
{
    m_langs = langs;

    if (!m_lang.isEmpty())
        emit curLangChanged(langByKey(m_lang));

    emit langChanged(langs);
}

void KeyboardModel::setCapsLock(bool value)
{
    if (m_capsLock != value) {
        m_capsLock = value;
        emit capsLockChanged(value);
    }
}

uint KeyboardModel::repeatDelay() const
{
    return m_repeatDelay;
}

void KeyboardModel::setRepeatDelay(const uint &repeatDelay)
{
    if (m_repeatDelay != repeatDelay) {
        m_repeatDelay = repeatDelay;
        emit repeatDelayChanged(repeatDelay);
    }
}

uint KeyboardModel::repeatInterval() const
{
    return m_repeatInterval;
}

void KeyboardModel::setRepeatInterval(const uint &repeatInterval)
{
    if (m_repeatInterval != repeatInterval) {
        m_repeatInterval = repeatInterval;
        emit repeatIntervalChanged(repeatInterval);
    }
}

void KeyboardModel::setAllShortcut(const QMap<QStringList,int> &map)
{
    m_shortcutMap = map;
}

int KeyboardModel::kbSwitch() const
{
    return m_kbSwitch;
}

void KeyboardModel::setKbSwitch(int kbSwitch)
{
    if (m_kbSwitch != kbSwitch) {
        m_kbSwitch = kbSwitch;
        emit kbSwitchChanged(kbSwitch);
    }
}

bool KeyboardModel::numLock() const
{
    return m_numLock;
}

void KeyboardModel::setNumLock(bool numLock)
{
    if (m_numLock != numLock) {
        m_numLock = numLock;
        emit numLockChanged(m_numLock);
    }
}

void KeyboardModel::cleanUserLayout()
{
    m_userLayout.clear();
}

QString KeyboardModel::curLang() const
{
    return langByKey(m_lang);
}

QMap<QString, QString> KeyboardModel::userLayout() const
{
    return m_userLayout;
}

QMap<QString, QString> KeyboardModel::kbLayout() const
{
    return m_layouts;
}

void KeyboardModel::addUserLayout(const QString &id, const QString &value)
{
    if (!m_userLayout.contains(id)) {
        m_userLayout.insert(id, value);
        emit userLayoutChanged(id, value);
    }
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
