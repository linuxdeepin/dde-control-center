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
class KeyboardWork;
struct ShortcutInfo;
class KeyboardControl;

class ShortcutContent  : public ContentWidget
{
    Q_OBJECT

public:
    explicit ShortcutContent(KeyboardWork* work, QWidget *parent = 0);
    void setBottomTip(ShortcutInfo *conflict);
    void setCurInfo(ShortcutInfo* info);
    void setConflictString(const QStringList& list);

signals:
    void shortcut(const QString &shortcut);

public slots:
    void onClick();
    void onReplace();
    void onKeyEvent(const bool state, const QString &keylist);

private:
    KeyboardWork* m_work;
    QLabel* m_bottomTip;
    KeyboardControl* m_control;
    ShortcutInfo* m_conflict;
    ShortcutInfo* m_curInfo;

    dcc::widgets::ButtonTuple *m_buttonTuple;
    TitleButtonItem* m_item;
};
}
}
#endif // SHORTCUTCONTENT_H
