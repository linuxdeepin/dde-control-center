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

#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

#include "shortcutitem.h"
#include "settingsitem.h"
#include "keyboardwork.h"

using namespace dcc::widgets;

class ShortcutKey;

namespace dcc {
namespace keyboard {
class CustomItem : public SettingsItem
{
    Q_OBJECT
public:
    explicit CustomItem(KeyboardWork *work, QWidget *parent = 0);
    void setInfo(ShortcutInfo * info);
    QString text() const;

public slots:
    void onKeyEvent(bool press, const QString &keylist);

signals:
    void shortcut(const QString& shortcut);

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    KeyboardWork* m_work;
    QLineEdit* m_shortcutEdit;
    QLabel* m_title;
    QRect m_rect;
    bool m_contain;
    ShortcutInfo* m_info;
    QString m_accels;
    ShortcutKey *m_shortKey;
};
}
}


#endif // CUSTOMITEM_H
