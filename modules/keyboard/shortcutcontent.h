#ifndef SHORTCUTCONTENT_H
#define SHORTCUTCONTENT_H

#include "contentwidget.h"

class ShortcutContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutContent(QWidget *parent = 0);

signals:
    void shortcutChanged(const QString& shortcut);
};

#endif // SHORTCUTCONTENT_H
