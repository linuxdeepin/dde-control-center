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

#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

#include "shortcutitem.h"
#include "widgets/settingsitem.h"
#include "keyboardwork.h"

using namespace dcc::widgets;

class ShortcutKey;

namespace dcc {
namespace keyboard {
class CustomItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit CustomItem(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setShortcut(const QString &shortcut);
    QString text() const;

Q_SIGNALS:
    void requestUpdateKey();

protected:
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    QLabel *m_title;
    QLineEdit* m_shortcutEdit;
    QString m_accels;
    ShortcutKey *m_shortKey;
};
}
}


#endif // CUSTOMITEM_H
