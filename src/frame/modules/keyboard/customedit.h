// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/lineeditwidget.h"
#include "shortcutitem.h"
#include "shortcutmodel.h"
#include "keyboardwork.h"
#include <QObject>

using namespace dcc;
using namespace dcc::widgets;

class WaylandGrab;

namespace dcc {
namespace keyboard{
struct ShortcutInfo;
class CustomItem;
class CustomEdit : public ContentWidget
{
    Q_OBJECT
public:
    explicit CustomEdit(ShortcutModel *model, QWidget *parent = 0);
    void setShortcut(ShortcutInfo* info);

Q_SIGNALS:
    void requestUpdateKey(ShortcutInfo *info);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void setBottomTip(ShortcutInfo *conflict);
    void keyEvent(bool press, const QString &shortcut);
    void onGrab(ShortcutInfo *info);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private Q_SLOTS:
    void onOpenFile();
    void onSaveAccels();
    void onUpdateKey();

private:
    ShortcutModel *m_model;
    SettingsGroup  *m_commandGroup;
    LineEditWidget *m_name;
    LineEditWidget *m_command;
    CustomItem   *m_short;
    ShortcutInfo   *m_info;
    QLabel         *m_tip;
    ShortcutInfo *m_conflict;
    WaylandGrab *m_waylandGrab;
    bool         m_nameChanged;
    bool         m_nameRepeated;
    bool         m_commandChanged;
    bool         m_accelsChanged;
};
}
}

#endif // CUSTOMEDIT_H
