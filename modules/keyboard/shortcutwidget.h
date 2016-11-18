#ifndef SHORTCUTWIDGET_H
#define SHORTCUTWIDGET_H

#include "contentwidget.h"
#include "shortcutmodel.h"
#include "settingsgroup.h"

using namespace dcc;

class ShortcutWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutWidget(QWidget *parent = 0);

    enum InfoType{
        System,
        Custom,
        Media,
        Window,
        Workspace
    };

    void addShortcut(QList<ShortcutInfo*> list, ShortcutWidget::InfoType type);

private:
    SettingsGroup* m_systemGroup;
    SettingsGroup* m_windowGroup;
    SettingsGroup* m_workspaceGroup;
    SettingsGroup* m_custonGroup;
};

#endif // SHORTCUTWIDGET_H
