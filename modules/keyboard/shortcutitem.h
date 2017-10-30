/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef SHORTCUTITEM_H
#define SHORTCUTITEM_H

#include "settingsitem.h"
#include "shortcutkey.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <dimagebutton.h>
#include <com_deepin_daemon_keybinding.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

struct ShortcutInfo;
class ShortcutModel;
using KeybingdingInter = com::deepin::daemon::Keybinding;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = 0);
    void setModel(ShortcutModel *model);
    void setShortcutInfo(ShortcutInfo* info);
    void displayConflict(bool display = false);
    ShortcutInfo* curInfo();

    void setChecked(bool checked);
    void setTitle(const QString &title);

signals:
    void shortcutChangd(bool valid, ShortcutInfo* info, const QString& shortcut);
    void requestDisableShortcut(ShortcutInfo* info);
    void shortcutEditChanged(ShortcutInfo* info);
    void requestUpdateKey(ShortcutInfo *info);

public slots:
    void onKeyEvent(const QString &shortcut);
    void onEditMode(bool value);
    void onChecked();

private:
    void onShortcutEdit();

private slots:
    void updateShortcutKeys();

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    QLineEdit* m_shortcutEdit;
    QLabel* m_title;
    QRect m_rect;
    bool m_contain;
    bool m_display;
    bool m_checked;
    ShortcutInfo* m_info;
    DImageButton* m_checkBtn;
    DImageButton* m_editBtn;
    ShortcutKey *m_key;
    ShortcutModel *m_model;
};
}
}
#endif // SHORTCUTITEM_H
