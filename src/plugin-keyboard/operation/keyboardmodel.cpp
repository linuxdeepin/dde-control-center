//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "keyboardmodel.h"
#include <QDebug>

using namespace dccV25;
KeyboardModel::KeyboardModel(QObject *parent)
    : QObject(parent)
    , m_keyboardEnabled(true)
    , m_capsLock(true)
    , m_numLock(true)
    , m_repeatInterval(1)
    , m_repeatDelay(1)
{
}

void KeyboardModel::setLangChangedState(const int state)
{
    if (m_status != state) {
        m_status = state;
        Q_EMIT onSetCurLangFinish(state);
    }
}

void KeyboardModel::setLayoutLists(QMap<QString, QString> lists)
{
    m_layouts = lists;
}

void KeyboardModel::setAllLayoutLists(QMap<QString, QString> lists)
{
    m_allLayouts = lists;
}

QString KeyboardModel::langByKey(const QString &key) const
{
    auto res = std::find_if(m_langList.cbegin(), m_langList.end(), [key] (const MetaData &data)->bool{
        return data.key() == key;
    });

    if (res != m_langList.cend()) {
        return res->text();
    }

    return QString();
}

QString KeyboardModel::langFromText(const QString &text) const
{
    auto res = std::find_if(m_langList.cbegin(), m_langList.end(), [text] (const MetaData &data)->bool{
        return data.text() == text;
    });

    if (res != m_langList.cend()) {
        return res->key();
    }

    return QString();
}

void KeyboardModel::setLayout(const QString &key)
{
    if (key.isEmpty())
        return;

    if (m_layout == key)
        return ;

    m_layout = key;

    Q_EMIT curLayoutChanged(m_layout);
}

QString KeyboardModel::curLayout() const
{
    return m_layout;
}

void KeyboardModel::setLang(const QString &value)
{
    qDebug() << "old key is " << m_currentLangKey << " new is " << value;
    if (m_currentLangKey != value && !value.isEmpty()) {
        m_currentLangKey = value;
        const QString &langName = langByKey(value);
        qDebug() << "value is " << value << " langName is " << langName;
        if (!langName.isEmpty())
            Q_EMIT curLangChanged(langName);
    }
}

QStringList KeyboardModel::convertLang(const QStringList &langList)
{
    QStringList realLangList;
    for (int i = 0; i < langList.size(); ++i) {
        QString lang = langByKey(langList[i]);
        if (!lang.isEmpty()) {
            realLangList << lang;
        }
    }
    return realLangList;
}

void KeyboardModel::setLocaleLang(const QStringList &localLangList)
{
    QStringList langList = convertLang(localLangList);
    if (m_localLangList != langList && !langList.isEmpty()) {
        m_localLangList = langList;

        Q_EMIT curLocalLangChanged(m_localLangList);
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

bool KeyboardModel::keyboardEnabled() const
{
    return m_keyboardEnabled;
}

void KeyboardModel::setKeyboardEnabled(bool keyboardEnabled)
{
    if (m_keyboardEnabled != keyboardEnabled) {
        m_keyboardEnabled = keyboardEnabled;
        Q_EMIT keyboardEnabledChanged(m_keyboardEnabled);
    }
}

void KeyboardModel::setAllShortcut(const QMap<QStringList, int> &map)
{
    m_shortcutMap = map;
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
    qDebug() << "curLang key is " << m_currentLangKey;
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

QStringList KeyboardModel::localLang() const
{
    return m_localLangList;
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
