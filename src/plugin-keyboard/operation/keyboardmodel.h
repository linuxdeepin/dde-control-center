//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef KEYBOARDMODEL_H
#define KEYBOARDMODEL_H

#include "metadata.h"

#include <QObject>
#include <QStringList>
#include <QMap>

namespace dccV25 {

class KeyboardModel : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardModel(QObject *parent = nullptr);
    enum KBLayoutScope {
        system = 0,
        application = 1
    };
#ifndef DCC_DISABLE_KBLAYOUT
    void setLayoutLists(QMap<QString, QString> lists);
    void setAllLayoutLists(QMap<QString, QString> lists);
#endif
    QString langByKey(const QString &key) const;
    QString langFromText(const QString &text) const;

    QString curLayout() const;
    QString curLang() const;
    QMap<QString, QString> userLayout() const;
    QMap<QString, QString> kbLayout() const;
    QMap<QString, QString> allLayout() const { return m_allLayouts; }
    QStringList localLang() const;
    QList<MetaData> langLists() const;
    bool capsLock() const;
    QMap<QStringList, int> allShortcut() const;

    uint repeatInterval() const;
    void setRepeatInterval(const uint &repeatInterval);

    bool keyboardEnabled() const;

    uint repeatDelay() const;
    void setRepeatDelay(const uint &repeatDelay);

    bool numLock() const;
    void setNumLock(bool numLock);

    void cleanUserLayout();

    inline int getLangChangedState() const { return m_status; }
    void setLangChangedState(const int state);
    inline QStringList &getUserLayoutList() { return m_userLaylist; }

Q_SIGNALS:
#ifndef DCC_DISABLE_KBLAYOUT
    void curLayoutChanged(const QString &layout);
#endif
    void keyboardEnabledChanged(bool value);
    void curLangChanged(const QString &lang);
    void capsLockChanged(bool value);
    void numLockChanged(bool value);
    void repeatDelayChanged(const uint value);
    void repeatIntervalChanged(const uint value);
    void userLayoutChanged(const QString &id, const QString &value);
    void langChanged(const QList<MetaData> &data);

    void curLocalLangChanged(const QStringList &localLangList);
    void onSetCurLangFinish(const int value);

public Q_SLOTS:
#ifndef DCC_DISABLE_KBLAYOUT
    void setLayout(const QString &key);
#endif
    void setLang(const QString &value);
    void setLocaleLang(const QStringList &localLangList);
    void addUserLayout(const QString &id, const QString &value);
    void setLocaleList(const QList<MetaData> &langList);
    void setCapsLock(bool value);
    void setAllShortcut(const QMap<QStringList, int> &map);
    void setKeyboardEnabled(bool keyboardEnabled);
private:
    QStringList convertLang(const QStringList &langList);
private:
    bool m_keyboardEnabled;
    bool m_capsLock;
    bool m_numLock;
    uint m_repeatInterval;
    uint m_repeatDelay;
    QString m_layout;
    QString m_currentLangKey;
    QStringList m_localLangList;
    QStringList m_userLaylist;
    QMap<QString, QString> m_userLayout;
    QMap<QString, QString> m_layouts;
    QMap<QString, QString> m_allLayouts;
    QList<MetaData> m_langList;
    QMap<QStringList, int> m_shortcutMap;
    int m_status{0};
};
}
#endif // KEYBOARDMODEL_H
