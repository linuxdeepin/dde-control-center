#ifndef KEYBAORDCONTROL_H
#define KEYBAORDCONTROL_H

#include "settingsitem.h"

using namespace dcc;

class KeyItem;
class KeyboardControl : public SettingsItem
{
    Q_OBJECT
public:
    explicit KeyboardControl(QFrame *parent = 0);
    ~KeyboardControl();

    void setConflictString(const QStringList& list);
protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void showEvent(QShowEvent* e);

    void append(KeyItem* item);
signals:
    void shortcutChanged(const QString& shortcut);

public slots:
    void onFocusWidget(QWidget *old, QWidget *now);
private:
    bool m_modifiers;
    int m_keycount;
    QList<KeyItem*> m_stack;
    KeyItem* m_last;
    QStringList m_conflicts;
};

#endif // KEYBAORDCONTROL_H
