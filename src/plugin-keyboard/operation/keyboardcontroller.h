//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include "keyboardmodel.h"
#include "keyboardwork.h"

#include <QObject>
#include <QSortFilterProxyModel>

namespace dccV25 {

class KeyboardController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint repeatInterval READ repeatInterval WRITE setRepeatInterval NOTIFY repeatIntervalChanged FINAL)
    Q_PROPERTY(uint repeatDelay READ repeatDelay WRITE setRepeatDelay NOTIFY repeatDelayChanged FINAL)
    Q_PROPERTY(bool numLock READ numLock WRITE setNumLock NOTIFY numLockChanged FINAL)
    Q_PROPERTY(bool capsLock READ capsLock WRITE setCapsLock NOTIFY capsLockChanged FINAL)
    Q_PROPERTY(int layoutCount READ layoutCount NOTIFY layoutCountChanged FINAL)
    Q_PROPERTY(QString currentLayout READ currentLayout WRITE setCurrentLayout NOTIFY currentLayoutChanged FINAL)
    Q_PROPERTY(QString conflictText READ conflictText WRITE setConflictText NOTIFY conflictTextChanged FINAL)

public:
    explicit KeyboardController(QObject *parent = nullptr);

    uint repeatInterval() const;
    void setRepeatInterval(uint newRepeatInterval);

    uint repeatDelay() const;
    void setRepeatDelay(uint newRepeatDelay);

    bool numLock() const;
    void setNumLock(bool newNumLock);

    bool capsLock() const;
    void setCapsLock(bool newCapsLock);

    int layoutCount() const;

    QString currentLayout() const;
    void setCurrentLayout(const QString &key);

    QString conflictText() const;
    void setConflictText(const QString &newConflictText);

public Q_SLOTS:
    void addUserLayout(const QString &layout);
    void deleteUserLayout(const QString &layout);

    QMap<QString, QString> userLayouts() const;
    QString userLayoutAt(int index, bool isValue = true) const;
    bool userLayoutsContains(const QString &key);
    QSortFilterProxyModel *layoutSearchModel();
    QSortFilterProxyModel *shortcutSearchModel();

    void updateKey(const QString &id, const int &type);
    QStringList formatKeys(const QString &shortcuts);

    void addCustomShortcut(const QString &name, const QString &cmd, const QString &accels);
    void modifyCustomShortcut(const QString &id, const QString &name, const QString &cmd, const QString &accels);
    void modifyShortcut(const QString &id, const QString &accels, const int &type);
    void deleteCustomShortcut(const QString &id);

    void resetAllShortcuts();

signals:
    void repeatIntervalChanged();
    void repeatDelayChanged();
    void numLockChanged();
    void capsLockChanged();
    void layoutCountChanged();
    void currentLayoutChanged();

    void requestRestore();
    void requestClear();
    void keyConflicted(const QString &oldAccels, const QString &newAccels);
    void keyDone(const QString &accels);
    void keyEvent(const QString &accels);

    void conflictTextChanged();

private:
    uint m_repeatInterval;
    uint m_repeatDelay;
    bool m_numLock;
    bool m_capsLock;
    KeyboardWorker *m_worker = nullptr;
    KeyboardModel *m_model = nullptr;
    ShortcutModel *m_shortcutModel = nullptr;
    QSortFilterProxyModel *m_layoutSearchModel = nullptr;
    QSortFilterProxyModel *m_shortcutSearchModel = nullptr;
    QString m_conflictText;
};

}
#endif // KEYBOARDCONTROLLER_H
