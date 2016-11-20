#ifndef KEYBAORDCONTROL_H
#define KEYBAORDCONTROL_H

#include <QFrame>

class KeyItem;
class KeyboardControl : public QFrame
{
    Q_OBJECT
public:
    explicit KeyboardControl(QFrame *parent = 0);
    ~KeyboardControl();

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

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
};

#endif // KEYBAORDCONTROL_H
