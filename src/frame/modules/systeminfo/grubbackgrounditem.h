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

#ifndef GRUBBACKGROUNDITEM_H
#define GRUBBACKGROUNDITEM_H

#include "widgets/settingsitem.h"

using namespace dcc::widgets;

namespace dcc{
namespace systeminfo{

class GrubBackgroundItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit GrubBackgroundItem(QFrame* parent = 0);

Q_SIGNALS:
    void requestEnableTheme(const bool state);
    void requestSetBackground(const QString &path);

public Q_SLOTS:
    void setThemeEnable(const bool state);
    void updateBackground(const QPixmap &pixmap);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *e) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;

private:
    QPixmap m_background;
    bool m_isDrop;
    bool m_themeEnable;
};

}
}
#endif // GRUBBACKGROUNDITEM_H
