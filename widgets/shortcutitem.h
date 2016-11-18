#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include "settingsitem.h"
#include <QHBoxLayout>
#include <QLabel>

using namespace dcc;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = 0);

    void setShortcutString(QString shortcut);
    void setTitle(const QString& title);

private:
    QHBoxLayout* m_hlayout;
    QLabel* m_title;
};

#endif // SHORTCUTITEM_H
