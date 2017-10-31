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

#ifndef SHORTCUTCONTENT_H
#define SHORTCUTCONTENT_H

#include "contentwidget.h"
#include "settingsgroup.h"

#include "titlebuttonItem.h"
#include <QPushButton>

#include "buttontuple.h"

using namespace dcc;

namespace dcc {
namespace keyboard{
class ShortcutModel;
struct ShortcutInfo;
class KeyboardControl;

class ShortcutContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutContent(ShortcutModel *model, QWidget *parent = 0);
    void setBottomTip(ShortcutInfo *conflict);
    void setInfo(ShortcutInfo *info);
    void setShortcut(const QString &shortcut);

signals:
    void requestUpdateKey(ShortcutInfo *conflict);
    void requestSaveShortcut(ShortcutInfo *info);
    void requestDisableShortcut(ShortcutInfo *info);

public slots:
    void keyEvent(bool press, const QString &shortcut);
    void onReplace();
    void onUpdateKey();

private:
    ShortcutModel *m_model;
    QLabel* m_bottomTip;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_info;
    dcc::widgets::ButtonTuple *m_buttonTuple;
    TitleButtonItem* m_item;
    QString m_shortcut;
};
}
}
#endif // SHORTCUTCONTENT_H
