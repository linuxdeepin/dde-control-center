#ifndef CUSTOMCONTENT_H
#define CUSTOMCONTENT_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "keyboardcontrol.h"
#include <QPushButton>

using namespace dcc;

namespace dcc {
namespace keyboard{

class KeyboardWork;
struct ShortcutInfo;
class InputItem;

class CustomContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit CustomContent(KeyboardWork* work, QWidget *parent = 0);

    void setBottomTip(ShortcutInfo *conflict);
    void setConflictString(const QStringList& list);

signals:
    void shortcut(const QString& shortcut);

public slots:
    void onClick();
    void onShortcut();
    void onKeyEvent(const bool state, const QString &keylist);

private:
    KeyboardWork* m_work;
    SettingsGroup *m_commandGroup;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    InputItem* m_name;
    InputItem* m_command;
    InputItem* m_shortcut;
    KeyboardControl* m_control;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;
};
}
}
#endif // CUSTOMCONTENT_H
