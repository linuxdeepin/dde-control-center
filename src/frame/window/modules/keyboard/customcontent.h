// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include <dfilechooseredit.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class WaylandGrab;

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
    void mousePressEvent(QMouseEvent *e) override;

private:
    dcc::keyboard::KeyboardWorker *m_work;
    dcc::keyboard::CustomItem *m_shortcut;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_shortCutNameEdit;
    DTK_WIDGET_NAMESPACE::DFileChooserEdit *m_shortCutCmdEdit;
    QLabel *m_bottomTip;
    dcc::keyboard::ShortcutInfo *m_conflict;
    dcc::keyboard::ShortcutModel *m_model;
    dcc::widgets::ButtonTuple *m_buttonTuple;

    WaylandGrab *m_waylandGrab;
};
}
}
