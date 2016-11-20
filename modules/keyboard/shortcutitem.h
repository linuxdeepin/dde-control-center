#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include "settingsitem.h"
#include <QHBoxLayout>
#include <QLabel>

using namespace dcc;

class QLineEdit;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = 0);

    void setShortcutString(QString shortcut);
    void setTitle(const QString& title);

signals:
    void shortcutChangd(const QString& shortcut);

protected:
    void mousePressEvent(QMouseEvent *e);

private:
    QLineEdit* m_shortcutEdit;
    QHBoxLayout* m_hlayout;
    QLabel* m_title;
};

#endif // SHORTCUTITEM_H
