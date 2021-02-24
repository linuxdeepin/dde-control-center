/*
 * Copyright (C) 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     liuxing <liuxing@uniontech.com>
 *
 * Maintainer: liuxing <liuxing@uniontech.com>
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

#ifndef PAPERICONBUTTON_H
#define PAPERICONBUTTON_H

#include "interface/namespace.h"

#include <DWidget>

namespace DCC_NAMESPACE {
namespace personalization {

class PaperIconButton : public Dtk::Widget::DWidget
{
    Q_OBJECT
public:
    explicit PaperIconButton(QWidget *parent = nullptr);
    void setIcon(const QPixmap &pix);
    void setOpacity(const int opacity);
    void setRadius(const int radius);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

Q_SIGNALS:
    void clicked();

private:
    QPixmap m_icon;
    int m_opacity;
    int m_radius;
    bool m_hover;
};
}
}

#endif // PAPERICONBUTTON_H
