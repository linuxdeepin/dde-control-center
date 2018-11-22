/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "widgets/settingsitem.h"
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
using KeybingdingInter = com::deepin::daemon::Keybinding;

class ShortcutItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit ShortcutItem(QFrame *parent = 0);

    void setShortcutInfo(ShortcutInfo* info);
    ShortcutInfo* curInfo();

    void setChecked(bool checked);
    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);

Q_SIGNALS:
    void shortcutEditChanged(ShortcutInfo* info);
    void requestUpdateKey(ShortcutInfo *info);
    void requestRemove(ShortcutInfo *info);

public Q_SLOTS:
    void onEditMode(bool value);
    void onRemoveClick();

private:
    void onShortcutEdit();
    void updateTitleSize();

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    QLineEdit* m_shortcutEdit;
    QLabel* m_title;
    ShortcutInfo* m_info;
    DImageButton* m_checkBtn;
    DImageButton* m_editBtn;
    ShortcutKey *m_key;
};
}
}
#endif // SHORTCUTITEM_H
