#ifndef SHORTCUTCONTENT_H
#define SHORTCUTCONTENT_H

#include "contentwidget.h"
#include "settingsgroup.h"

#include "inputitem.h"
#include <QPushButton>

using namespace dcc;

class KeyboardWork;
class ShortcutInfo;
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
    void shortcutChanged(const QString& shortcut);

public slots:
    void onClick();
    void onReplace();
    void setShortcut(const QString& shortcut);
    void onKeyEvent(bool press, QString shortcut);
private:
    KeyboardWork* m_work;
    QPushButton* m_ok;
    QPushButton* m_cancel;
    QLabel* m_bottomTip;
    KeyboardControl* m_control;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_curInfo;
};

#endif // SHORTCUTCONTENT_H
