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
#include "waylandgrab.h"

#include <dfilechooseredit.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace keyboard {
class ShortcutModel;
struct ShortcutInfo;
class KeyboardWorker;
class CustomItem;
}

namespace widgets {
class SettingsGroup;
class LineEditWidget;
class ButtonTuple;
}
}

namespace DCC_NAMESPACE {
namespace keyboard {
class CustomContent  : public QWidget
{
    Q_OBJECT

public:
    explicit CustomContent(dcc::keyboard::ShortcutModel *model, QWidget *parent = nullptr);
    void setBottomTip(dcc::keyboard::ShortcutInfo *conflict);
Q_SIGNALS:
    void back() const;
    void requestAddKey(const QString &name, const QString &command, const QString &accels);
    void requestUpdateKey(dcc::keyboard::ShortcutInfo *info);
    void requestForceSubs(dcc::keyboard::ShortcutInfo *info);
    void requestFrameAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void onShortcut();
    void keyEvent(bool press, const QString &shortcut);
    void updateKey();

    void onGrab(dcc::keyboard::ShortcutInfo *info);

protected:
    void keyPressEvent(QKeyEvent *ke) override;
    void keyReleaseEvent(QKeyEvent *ke) override;

private:
    dcc::keyboard::KeyboardWorker *m_work;
    dcc::keyboard::CustomItem *m_shortcut;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_shortCutNameEdit;
    DTK_WIDGET_NAMESPACE::DFileChooserEdit *m_shortCutCmdEdit;
    QLabel *m_bottomTip;
    dcc::keyboard::ShortcutInfo *m_conflict;
    dcc::keyboard::ShortcutModel *m_model;
    dcc::widgets::ButtonTuple *m_buttonTuple;

    WaylandGrab *waylandGrab = nullptr;
};
}
}
