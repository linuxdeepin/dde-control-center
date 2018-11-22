/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keyboardmodel.h"
#include <QDebug>

namespace dcc {
namespace keyboard{
KeyboardModel::KeyboardModel(QObject *parent)
    : QObject(parent)
    , m_capsLock(true)
    , m_numLock(true)
    , m_repeatInterval(1)
    , m_repeatDelay(1)
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
    for (const auto &lang : m_langList)
        if (lang.key() == key)
            return lang.text();

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

    Q_EMIT curLayoutChanged(m_layout);
}
#endif

QString KeyboardModel::curLayout() const
{
    return m_layout;
}

void KeyboardModel::setLang(const QString &value)
{
    if (m_currentLangKey != value && !value.isEmpty())
    {
        m_currentLangKey = value;

        const QString &langName = langByKey(value);
        if (!langName.isEmpty())
            Q_EMIT curLangChanged(langName);
    }
}

void KeyboardModel::setLocaleList(const QList<MetaData> &langList)
{
    if (langList.isEmpty())
        return;

    m_langList = langList;
    Q_EMIT langChanged(langList);

    const QString &currentLang = langByKey(m_currentLangKey);
    if (!currentLang.isEmpty())
        Q_EMIT curLangChanged(currentLang);
}

void KeyboardModel::setCapsLock(bool value)
{
    if (m_capsLock != value) {
        m_capsLock = value;
        Q_EMIT capsLockChanged(value);
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
        Q_EMIT repeatDelayChanged(repeatDelay);
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
        Q_EMIT repeatIntervalChanged(repeatInterval);
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
        Q_EMIT kbSwitchChanged(kbSwitch);
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
        Q_EMIT numLockChanged(m_numLock);
    }
}

void KeyboardModel::cleanUserLayout()
{
    m_userLayout.clear();
}

QString KeyboardModel::curLang() const
{
    return langByKey(m_currentLangKey);
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
        Q_EMIT userLayoutChanged(id, value);
    }
}

QList<MetaData> KeyboardModel::langLists() const
{
    return m_langList;
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
