#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include "settingsitem.h"
#include <QHBoxLayout>
#include <QLabel>
#include <dimagebutton.h>
#include <com_deepin_daemon_keybinding.h>

DWIDGET_USE_NAMESPACE

using KeybingdingInter = com::deepin::daemon::Keybinding;

using namespace dcc;

class QLineEdit;
class ShortcutInfo;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = 0);
    void setShortcutInfo(ShortcutInfo* info);
    void displayConflict(bool display = false);
    ShortcutInfo* curInfo();

    void setChecked(bool checked);

signals:
    void shortcutChangd(bool valid, ShortcutInfo* info, const QString& shortcut);
    void checkedChanged(bool checked);

public slots:
    void onFocusChanged(QWidget* old, QWidget *now);
    void onKeyEvent(bool press, QString shortcut);
    void onEditMode(bool value);
    void onChecked();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void leaveEvent(QEvent *);

private:
    QLineEdit* m_shortcutEdit;
    QLabel* m_title;
    QRect m_rect;
    bool m_contain;
    bool m_display;
    bool m_checked;
    ShortcutInfo* m_info;
    KeybingdingInter* m_inter;
    DImageButton* m_checkBtn;
};
#endif // SHORTCUTITEM_H
