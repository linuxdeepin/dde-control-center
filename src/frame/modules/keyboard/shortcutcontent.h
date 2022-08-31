// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SHORTCUTCONTENT_H
#define SHORTCUTCONTENT_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"

#include "titlebuttonItem.h"
#include <QPushButton>

#include "widgets/buttontuple.h"

using namespace dcc;
class WaylandGrab;

namespace dcc {
namespace keyboard{
class ShortcutModel;
struct ShortcutInfo;
class KeyboardControl;
class ShortcutItem;
class ShortcutContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutContent(ShortcutModel *model, QWidget *parent = 0);
    void setBottomTip(ShortcutInfo *conflict);
    void setInfo(ShortcutInfo *info);
    void setShortcut(const QString &shortcut);
    void setConflictShortcut(const QString &shortcut);

Q_SIGNALS:
    void requestUpdateKey(ShortcutInfo *conflict);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestDisableShortcut(ShortcutInfo *info);

public Q_SLOTS:
    void keyEvent(bool press, const QString &shortcut);
    void onReplace();
    void onUpdateKey();
    void onGrab(ShortcutInfo *info);

protected:
    void keyPressEvent(QKeyEvent *ke) override;
    void keyReleaseEvent(QKeyEvent *ke) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    ShortcutModel *m_model;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_info;
    ShortcutItem *m_shortcutItem;
    dcc::widgets::ButtonTuple *m_buttonTuple;
    TitleButtonItem* m_item;
    QString m_shortcut;
    QString m_conflictShortcut;
    WaylandGrab *m_waylandGrab;
};
}
}
#endif // SHORTCUTCONTENT_H
