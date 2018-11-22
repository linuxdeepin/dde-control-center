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

#ifndef KEYBAORDCONTROL_H
#define KEYBAORDCONTROL_H

#include "widgets/settingsitem.h"

using namespace dcc::widgets;

namespace dcc {
namespace keyboard{

class KeyItem;
class KeyboardControl : public SettingsItem
{
    Q_OBJECT
public:
    explicit KeyboardControl(QFrame *parent = 0);
    ~KeyboardControl();

    void setConflictString(const QStringList& list);
    void setPress(const QString& key, bool press);
    void setConflicts(const QString& key, bool press);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool m_modifiers;
    int m_keycount;
    QList<KeyItem*> m_stack;
    KeyItem* m_last;
    QStringList m_conflicts;
};

}
}
#endif // KEYBAORDCONTROL_H
