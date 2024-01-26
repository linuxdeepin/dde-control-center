//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include <DFileChooserEdit>
#include <DAbstractDialog>
#include <QWidget>

class QLabel;
DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {
class ShortcutModel;
struct ShortcutInfo;
class KeyboardWorker;
class CustomItem;

class SettingsGroup;
class LineEditWidget;
class ButtonTuple;

class CustomContentDialog  : public DAbstractDialog
{
    Q_OBJECT
public:
    explicit CustomContentDialog(ShortcutModel *model, QWidget *parent = nullptr);
    void setBottomTip(ShortcutInfo *conflict);

protected:
    void keyPressEvent(QKeyEvent *e) override;

Q_SIGNALS:
    void requestAddKey(const QString &name, const QString &command, const QString &accels);
    void requestUpdateKey(ShortcutInfo *info);
    void requestForceSubs(ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void onShortcut();
    void keyEvent(bool press, const QString &shortcut);
    void updateKey();

private:
    KeyboardWorker *m_work;
    CustomItem *m_shortcut;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_shortCutNameEdit;
    DTK_WIDGET_NAMESPACE::DFileChooserEdit *m_shortCutCmdEdit;
    QLabel *m_bottomTip;
    ShortcutInfo *m_conflict;
    ShortcutModel *m_model;
    ButtonTuple *m_buttonTuple;
};
}
