//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SHORTCUTCONTENTDIALOG_H
#define SHORTCUTCONTENTDIALOG_H

#include "interface/namespace.h"
#include "widgets/settingsgroup.h"
#include "widgets/buttontuple.h"
#include "titlebuttonItem.h"

#include <DAbstractDialog>

#include <QPushButton>

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class ShortcutModel;
struct ShortcutInfo;
class KeyboardControl;
class ShortcutItem;
class ShortcutContentDialog  : public DAbstractDialog
{
    Q_OBJECT
public:
    explicit ShortcutContentDialog(ShortcutModel *model, QWidget *parent = nullptr);
    void setBottomTip(ShortcutInfo *conflict);
    void setInfo(ShortcutInfo *info);
    void setShortcut(const QString &shortcut);

Q_SIGNALS:
    void requestUpdateKey(ShortcutInfo *conflict);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestDisableShortcut(ShortcutInfo *info);

public Q_SLOTS:
    void keyEvent(bool press, const QString &shortcut);
    void onReplace();
    void onUpdateKey();

private:
    ShortcutModel *m_model;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_info;
    ShortcutItem *m_shortcutItem;
    ButtonTuple *m_buttonTuple;
    TitleButtonItem* m_item;
    QString m_shortcut;
};
}
#endif // SHORTCUTCONTENTDIALOG_H
