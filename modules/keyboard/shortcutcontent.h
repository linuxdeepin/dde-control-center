#ifndef SHORTCUTCONTENT_H
#define SHORTCUTCONTENT_H

#include "contentwidget.h"
#include "settingsgroup.h"

#include "titlebuttonItem.h"
#include <QPushButton>

#include "buttontuple.h"

using namespace dcc;

namespace dcc {
namespace keyboard{
class KeyboardWork;
struct ShortcutInfo;
class KeyboardControl;

class ShortcutContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutContent(KeyboardWork* work, QWidget *parent = 0);
    void setBottomTip(ShortcutInfo *conflict);
    void setCurInfo(ShortcutInfo* info);
    void setConflictString(const QStringList& list);

signals:
    void shortcut(const QString &shortcut);

public slots:
    void onClick();
    void onReplace();
    void onKeyEvent(const bool state, const QString &keylist);

private:
    KeyboardWork* m_work;
    QLabel* m_bottomTip;
    KeyboardControl* m_control;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_curInfo;

    dcc::widgets::ButtonTuple *m_buttonTuple;
    TitleButtonItem* m_item;
};
}
}
#endif // SHORTCUTCONTENT_H
