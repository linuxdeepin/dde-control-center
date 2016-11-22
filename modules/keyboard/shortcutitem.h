#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include "settingsitem.h"
#include "shortcutmodel.h"
#include <QHBoxLayout>
#include <QLabel>
#include <com_deepin_daemon_keybinding.h>
using KeybingdingInter = com::deepin::daemon::Keybinding;

using namespace dcc;

class QLineEdit;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = 0);
    void setShortcutInfo(ShortcutInfo* info);

signals:
    void shortcutChangd(bool valid, ShortcutInfo* info, const QString& shortcut);

public slots:
    void onFocusChanged(QWidget* old, QWidget *now);
    void onKeyEvent(bool in0, QString in1);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *);

private:
    QLineEdit* m_shortcutEdit;
    QHBoxLayout* m_hlayout;
    QLabel* m_title;
    QRect m_rect;
    bool m_contain;
    ShortcutInfo* m_info;
    KeybingdingInter* m_inter;
};
#endif // SHORTCUTITEM_H
