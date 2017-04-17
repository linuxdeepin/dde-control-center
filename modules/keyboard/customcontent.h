#ifndef CUSTOMCONTENT_H
#define CUSTOMCONTENT_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "keyboardcontrol.h"
#include <QPushButton>

#include "buttontuple.h"
#include "lineeditwidget.h"
#include "customitem.h"

using namespace dcc;

namespace dcc {
namespace keyboard{

class KeyboardWork;
struct ShortcutInfo;

class CustomContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit CustomContent(KeyboardWork* work, QWidget *parent = 0);

    void setBottomTip(ShortcutInfo *conflict);
    void setConflictString(const QStringList& list);

signals:
    void shortcut(const QString& shortcut);
    void requestFrameAutoHide(const bool autoHide) const;

public slots:
    void onShortcut();

private slots:
    void onOpenFile();

private:
    KeyboardWork* m_work;
    SettingsGroup *m_commandGroup;
    LineEditWidget* m_name;
    LineEditWidget* m_command;
    CustomItem *m_shortcut;
    KeyboardControl* m_control;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;

    dcc::widgets::ButtonTuple *m_buttonTuple;
};
}
}
#endif // CUSTOMCONTENT_H
