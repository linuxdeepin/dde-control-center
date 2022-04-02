/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
Q_SIGNALS:
    void requestCloseDlg();
    void requestAddKey(const QString &name, const QString &command, const QString &accels);
    void requestUpdateKey(ShortcutInfo *info);
    void requestForceSubs(ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void onShortcut();
    void keyEvent(bool press, const QString &shortcut);
    void updateKey();

protected:
    void closeEvent(QCloseEvent *event) override;

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
