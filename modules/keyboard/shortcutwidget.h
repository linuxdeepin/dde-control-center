#ifndef SHORTCUTWIDGET_H
#define SHORTCUTWIDGET_H

#include "contentwidget.h"
#include "shortcutmodel.h"
#include "settingsgroup.h"
#include "searchinput.h"

using namespace dcc;

class QLineEdit;

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

signals:
    void shortcutChanged(bool valid, ShortcutInfo* info, const QString& shortcut);
    void customShortcut();

public slots:
    void onSearch(const QString &text);

private:
    QLineEdit* m_search;
    SettingsGroup* m_systemGroup;
    SettingsGroup* m_windowGroup;
    SettingsGroup* m_workspaceGroup;
    SettingsGroup* m_custonGroup;
    SettingsGroup* m_searchGroup;
};

#endif // SHORTCUTWIDGET_H
