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

#ifndef NEXTPAGEWIDGET_H
#define NEXTPAGEWIDGET_H

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"
#include "widgets/labels/tipslabel.h"
#include "nextbutton.h"

#include <QLabel>

namespace dcc {
namespace widgets {

class NextPageWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit NextPageWidget(QFrame *parent = nullptr, bool bLeftInterval = true);

    inline QString title() const { return m_title->text(); }
    void setTitle(const QString &title);
    void setValue(const QString &value);
    void setIcon(const QPixmap &icon);

    void clearValue() { m_value->clear(); }
    inline QString value() { return m_value->text();}

    void setRightTxtWordWrap(bool state = false);

protected:
    void mouseReleaseEvent(QMouseEvent *e);

Q_SIGNALS:
    void clicked() const;
    void selected() const;
    void acceptNextPage() const;

protected:
    NormalLabel *m_title;
    TipsLabel *m_value;
    NextButton *m_nextPageBtn;
};

}
}
#endif // NEXTPAGEWIDGET_H
