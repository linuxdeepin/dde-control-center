//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include "interface/namespace.h"
#include "widgets/settingsgroup.h"
#include "widgets/lineeditwidget.h"
#include "shortcutitem.h"
#include "operation/shortcutmodel.h"
#include <QObject>
#include <DAbstractDialog>

namespace DCC_NAMESPACE {
struct ShortcutInfo;
class CustomItem;
class CustomEdit : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit CustomEdit(ShortcutModel *model, QWidget *parent = nullptr);
    void setShortcut(ShortcutInfo* info);

Q_SIGNALS:
    void requestUpdateKey(ShortcutInfo *info);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void setBottomTip(ShortcutInfo *conflict);
    void keyEvent(bool press, const QString &shortcut);

private Q_SLOTS:
    void onOpenFile();
    void onSaveAccels();
    void onUpdateKey();

private:
    ShortcutModel   *m_model;
    SettingsGroup   *m_commandGroup;
    LineEditWidget  *m_name;
    LineEditWidget  *m_command;
    CustomItem      *m_short;
    ShortcutInfo    *m_info;
    QLabel          *m_tip;
    ShortcutInfo    *m_conflict;
};
}

#endif // CUSTOMEDIT_H
