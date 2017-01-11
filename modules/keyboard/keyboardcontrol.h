#ifndef KEYBAORDCONTROL_H
#define KEYBAORDCONTROL_H

#include "settingsitem.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

class KeyItem;
class KeyboardControl : public SettingsItem
{
    Q_OBJECT
public:
    explicit KeyboardControl(QFrame *parent = 0);
    ~KeyboardControl();

    void setConflictString(const QStringList& list);
    void setPress(const QString& key, bool press);
    void setConflicts(const QString& key, bool press);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool m_modifiers;
    int m_keycount;
    QList<KeyItem*> m_stack;
    KeyItem* m_last;
    QStringList m_conflicts;
};

}
}
#endif // KEYBAORDCONTROL_H
